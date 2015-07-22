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
        for(int v = 0; v<vertices.size(); v++) {
            cv::Point v0 = vertices[v].toMatFrame(dest, scale);
            cv::Scalar vertColor = cv::Scalar(0,255,0);
            cv::circle(dest, v0, 5, vertColor);

        }

        for(int e = 0; e<edges.size(); e++) {
            cv::Scalar edgeColor = cv::Scalar(0,255,0);
            cv::Point v0 = vertices[edges[e].verticesIdx[0]].toMatFrame(dest, scale);
            cv::Point v1 = vertices[edges[e].verticesIdx[1]].toMatFrame(dest, scale);
            cv::line(dest, v0, v1, edgeColor);


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
    int selectVertex(Vertex cur, int & selectedVertex) {
        std::map<int, Vertex>::iterator it;
        for(it = map.vertices.begin(); it!=map.vertices.end(); it++) {
            if(it->second.dist(cur) < selectThres) {
                selectedVertex = it->first;
                return 1;
            }
        }
		selectedVertex = -1;
		return 0;

	}


	Editor() {
		state = STATE_IDLE;
		activePolygon = -1;
		activeEdge = -1;
		activeVertex = -1;	
		viewport = cv::Mat::zeros(600,600,CV_8UC3);
		scale = 10;
		selectThres = 0.2f;
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
			int selectedVertex = 0;
            int selRes = selectVertex(cur, selectedVertex);

            if(state == STATE_IDLE) {
                if(selRes) {
                    activeVertex = selectedVertex;
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
            }
		}
		else if  ( event == EVENT_RBUTTONDOWN )
		{
            if(state == STATE_CREATE) {
                removeVertex(activeVertex);
                activeVertex = -1;
                state = STATE_IDLE;
            }

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
            }
		}
		
		clearViewport();

		map.draw(viewport, scale, activePolygon, activeEdge, activeVertex);
		cv::imshow("Editor", viewport);
	}
	void clearViewport() {
		viewport = cv::Scalar(50,50,50);//cv::Mat::zeros(600,600,CV_8UC3);
	}

};

int main(int argc, char** argv)
{
	Editor ed;
	ed.start();

	while(1) {cv::waitKey(1);}
	

    return 0;

}
