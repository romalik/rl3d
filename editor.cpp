#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace std;
using namespace cv;


struct Vertex {
	float x;
	float y;
	cv::Point toMatFrame(cv::Mat & dest, float scale) {
		cv::Point res;
		res.x = x * scale + dest.cols/2;
		res.y = -y * scale + dest.rows/2;
		return res;
	}
	float dist(Vertex v) {
		return sqrt( (v.x - x)*(v.x - x) + (v.y - y)*(v.y - y) );
	}

};

struct Edge {

    std::vector<int> verticesIdx;
    std::string texName;

};

struct Polygon {

	int closed;
	Polygon() {
		closed = 0;
	}

	std::vector<Edge> edges;
};

struct Map {

    std::map<int, Vertex> vertices;
    std::vector<Edge> edges;
    std::vector<Polygon> polygons;

	void draw(cv::Mat & dest, float scale, int selectedPolygon, int selectedEdge, int selectedVertex ) {
        std::map<int, Vertex>::iterator it = vertices.begin();
        for(; it != vertices.end(); it++) {
            cv::Point v0 = (*it).second.toMatFrame(dest, scale);
            cv::Scalar vertColor = cv::Scalar(0,255,0);
            if(it->first == selectedVertex)
                vertColor = cv::Scalar(0,0,255);
            cv::circle(dest, v0, 5, vertColor);

        }

        for(int e = 0; e<edges.size(); e++) {
            cv::Scalar edgeColor = cv::Scalar(0,255,0);
            int thick = 1;
            if( e == selectedEdge) {
                edgeColor = cv::Scalar(0,0,255);
                thick = 3;
            }
            cv::Point v0 = vertices[edges[e].verticesIdx[0]].toMatFrame(dest, scale);
            cv::Point v1 = vertices[edges[e].verticesIdx[1]].toMatFrame(dest, scale);
            cv::line(dest, v0, v1, edgeColor, thick);


        }
	}
	
};

class Editor {
public:
	Map map;
	cv::Mat viewport;
	float scale;

	int activePolygon;
	int activeEdge;
	int activeVertex;

	enum {
		STATE_IDLE = 0,
		STATE_CREATE,
		STATE_MOVE
	};


	int state;
	float selectThres;
    int selectVertex(Vertex cur, int & selectedVertex, int & selectedEdge, int maskedId = -1) {
        std::map<int, Vertex>::iterator it;
        for(it = map.vertices.begin(); it!=map.vertices.end(); it++) {
            if(it->second.dist(cur) < selectThres && it->first != maskedId) {
                selectedVertex = it->first;
                return 1;
            }
        }
		selectedVertex = -1;

        for(int e = 0; e<map.edges.size(); e++) {
            //vertices[edges[e].verticesIdx[0]] - vertices[edges[e].verticesIdx[1]]
            float x0 = cur.x;
            float y0 = cur.y;
            float x1 = map.vertices[map.edges[e].verticesIdx[0]].x;
            float y1 = map.vertices[map.edges[e].verticesIdx[0]].y;
            float x2 = map.vertices[map.edges[e].verticesIdx[1]].x;
            float y2 = map.vertices[map.edges[e].verticesIdx[1]].y;

            float dist = fabs((y2-y1)*x0 - (x2-x1)*y0 + x2*y1 - y2*x1) / sqrt((y2-y1)*(y2-y1) + (x2-x1)*(x2-x1));
            if(dist < selectThres) {
                selectedEdge = e;
                return 2;
            }
        }
        selectedEdge = -1;
		return 0;

	}


	Editor() {
		state = STATE_IDLE;
		activePolygon = -1;
		activeEdge = -1;
		activeVertex = -1;	
		viewport = cv::Mat::zeros(600,600,CV_8UC3);
        scale = 20;
        selectThres = 0.4f;
        textEditMode = 0;
        textDest = 0;
        cTextPos = 0;

	}

	void start() {
		namedWindow("Editor", 1);
		setMouseCallback("Editor", Editor::mouseEvtWrapper, this);
		imshow("Editor", viewport);
	}

	static void mouseEvtWrapper(int event, int x, int y, int flags, void* _ctx) {
		Editor * ctx = (Editor *)_ctx;
		ctx->mouseEvt(event, x, y, flags);
	}

    void replaceVertex(int from, int to) {
        for(int i = 0; i<map.edges.size(); i++) {
            for(int v = 0; v<2; v++) {
                if(map.edges[i].verticesIdx[v] == from) {
                    map.edges[i].verticesIdx[v] = to;
                }
            }
        }

    }

    void removeVertex(int idx) {
        map.vertices.erase(idx);
        while(1) {
            int flOk = 1;
            for(int i = 0; i<map.edges.size(); i++) {
                if(map.edges[i].verticesIdx[0] == idx || map.edges[i].verticesIdx[1] == idx) {
                    map.edges.erase(map.edges.begin() + i);
                    flOk = 0;
                    break;
                }
            }
            if(flOk) break;
        }


    }

	void mouseEvt(int event, int x, int y, int flags) {

		Vertex cur;
		cur.x = (float)(x - viewport.cols/2) / scale;
		cur.y = -(float)(y - viewport.rows/2) / scale;

		if  ( event == EVENT_LBUTTONDOWN )
		{
            textEditMode = 0;
            int selectedVertex = -1;
            int selectedEdge = -1;
            int selRes = selectVertex(cur, selectedVertex, selectedEdge, activeVertex);

            if(state == STATE_IDLE) {
                if(selRes == 1) {
                    activeVertex = selectedVertex;
                    activeEdge = -1;
                    state = STATE_MOVE;
                    textEditMode = 0;
                } else if (selRes == 2) {
                    activeEdge = selectedEdge;

                } else {
                    int vertUid = 0;
                    if(map.vertices.size()) {
                        vertUid = map.vertices.rbegin()->first + 1;
                    }
                    map.vertices[vertUid] = cur;
                    map.vertices[vertUid + 1] = cur;
                    Edge newEdge;
                    newEdge.verticesIdx.push_back(vertUid);
                    newEdge.verticesIdx.push_back(vertUid + 1);
                    map.edges.push_back(newEdge);
                    activeVertex = vertUid + 1;
                    activeEdge = -1;
                    state = STATE_CREATE;
                }
            } else if(state == STATE_CREATE) {
                Edge newEdge;
                newEdge.verticesIdx.push_back(activeVertex);
                int vertUid = 0;
                if(map.vertices.size()) {
                    vertUid = map.vertices.rbegin()->first + 1;
                }
                map.vertices[vertUid] = cur;
                newEdge.verticesIdx.push_back(vertUid);
                map.edges.push_back(newEdge);
                activeVertex = vertUid;
                activeEdge = -1;
            } else if(state == STATE_MOVE) {

                if(selRes == 1) {
                    replaceVertex(activeVertex, selectedVertex);
                    removeVertex(activeVertex);
                }


                state = STATE_IDLE;
                activeVertex = -1;
                activeEdge = -1;
            }
		}
		else if  ( event == EVENT_RBUTTONDOWN )
		{
            if(state == STATE_CREATE) {
                removeVertex(activeVertex);
                activeVertex = -1;
                activeEdge = -1;
                state = STATE_IDLE;
            }

            textEditMode = 0;
		}
		else if  ( event == EVENT_MBUTTONDOWN )
		{
		}
		else if ( event == EVENT_MOUSEMOVE )
		{

			if(state == STATE_CREATE) {
                if(activeVertex != -1) {
					printf("Move one edge\n");
                    map.vertices[activeVertex] = cur;
				}
            } else if(state == STATE_MOVE) {
                if(activeVertex != -1) {
                    map.vertices[activeVertex] = cur;
                }
            }
        }
        refresh();
	}
	void clearViewport() {
		viewport = cv::Scalar(50,50,50);//cv::Mat::zeros(600,600,CV_8UC3);
	}

    void refresh() {
        clearViewport();



        map.draw(viewport, scale, activePolygon, activeEdge, activeVertex);

        if(activeEdge != -1) {
            char edgeProps[1000];
            sprintf(edgeProps, "Edge #%d, texName: %s", activeEdge, map.edges[activeEdge].texName.c_str());
            cv::putText(viewport, std::string(edgeProps), cv::Point(10,10), FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(255,255,255) );
        }
        if(textEditMode != 0) {
            char destText[1000];
            if(textDest == TEXT_TEXTURE) {
                sprintf(destText, "Edit Texture name for edge #%d:", activeEdge);
                cv::putText(viewport, std::string(destText), cv::Point(10,viewport.rows - 50), FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(255,255,255) );
            }
            char nTextBuf[1000];
            memcpy(nTextBuf, textBuf, cTextPos);
            nTextBuf[cTextPos] = 0;
            cv::putText(viewport, std::string(nTextBuf), cv::Point(10,viewport.rows - 20), FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(255,255,255) );


        }

        cv::imshow("Editor", viewport);
    }

    int textEditMode;
    enum {
        TEXT_NONE = 0,
        TEXT_TEXTURE,
        TEXT_OTHER
    };
    int textDest;
    char textBuf[1000];
    int cTextPos;

    void dump() {
        for(int i = 0; i<map.edges.size(); i++) {
            Vertex v0 = map.vertices[map.edges[i].verticesIdx[0]];
            Vertex v1 = map.vertices[map.edges[i].verticesIdx[1]];
            printf("map.push_back(Edge(%ff, %ff, %ff, %ff, %s));\n", v0.x, v0.y, v1.x, v1.y, map.edges[i].texName.c_str());
//            printf("(%f,%f) - (%f,%f) : %s\n", v0.x, v0.y, v1.x, v1.y, map.edges[i].texName.c_str());
        }
    }

    int onKey(char  k) {
        if(textEditMode == 0) {
            if(k == 'q') return 1;
            if(k == 's') {
                if(activeEdge != -1) {
                    float x1 = map.vertices[map.edges[activeEdge].verticesIdx[0]].x;
                    float y1 = map.vertices[map.edges[activeEdge].verticesIdx[0]].y;
                    float x2 = map.vertices[map.edges[activeEdge].verticesIdx[1]].x;
                    float y2 = map.vertices[map.edges[activeEdge].verticesIdx[1]].y;

                    Vertex newVert;
                    newVert.x = (x1+x2)/2.0f;
                    newVert.y = (y1+y2)/2.0f;
                    int vertUid = map.vertices.rbegin()->first + 1;
                    map.vertices[vertUid] = newVert;

                    Edge edgeA;
                    Edge edgeB;

                    edgeA.verticesIdx.push_back(map.edges[activeEdge].verticesIdx[0]);
                    edgeA.verticesIdx.push_back(vertUid);

                    edgeB.verticesIdx.push_back(vertUid);
                    edgeB.verticesIdx.push_back(map.edges[activeEdge].verticesIdx[1]);

                    map.edges.push_back(edgeA);
                    map.edges.push_back(edgeB);

                    map.edges.erase(map.edges.begin() + activeEdge);
                }
                refresh();
            }
            if(k == 'd') {
                if(activeVertex != -1) {
                    removeVertex(activeVertex);
                    state = STATE_IDLE;
                }
            }
            if(k == 't') {
                textEditMode = 1;
                cTextPos = 0;
                textDest = TEXT_TEXTURE;

            }
            if(k == 'a') {
                dump();
            }
        } else {
            if( k == 13 || k == 10 ) {
                textBuf[cTextPos] = 0;
                if(textDest == TEXT_TEXTURE) {
                    map.edges[activeEdge].texName = std::string(textBuf);
                }
                cTextPos = 0;
                textEditMode = 0;
            } else if( k > 0 ) {
                printf("in text %d\n", k);
                textBuf[cTextPos] = k;
                cTextPos++;
            }
        }
        refresh();
        return 0;
    }
};

int main(int argc, char** argv)
{
	Editor ed;
	ed.start();

    while(1) {char c = cv::waitKey(1); if(c) if(ed.onKey(c)) break;}
	

    return 0;

}
