#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/core/types_c.h>



#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <stdio.h>
#include <sys/time.h>


struct Intersection {
    int type;
    float offset;
    float distance;
};

typedef std::map<int, std::vector<Intersection> > IntersectionMap;
bool sorter(Intersection i, Intersection j) { return i.distance < j.distance; }

void getAllIntersections(cv::Mat & map, float x, float y, float yaw, float fov, int nBeams, IntersectionMap & ms) {
    for(int i = 0; i<nBeams; i++) {
        float cBeamAngle = yaw + (nBeams/2 - i) * (fov / nBeams);

        while(cBeamAngle < -M_PI) cBeamAngle += 2.0f*M_PI;
        while(cBeamAngle > M_PI) cBeamAngle -= 2.0f*M_PI;

        //get intersections with vertical lines
        for(int n = 0; n < map.cols; n++) {
            float dist = ((float)n - x)/cos(cBeamAngle);
            if(dist < 0)
                continue;

            float xi = n;
            float yi = y + ((float)n-x)*sin(cBeamAngle)/cos(cBeamAngle);
            int xm = (int)xi;

            if(cos(cBeamAngle) < 0)
                xm--;

            int ym = - (int)yi;

            if(xm < 0 || xm >= map.cols || ym < 0 || ym >= map.rows)
                continue;

            //printf("%d %d\n", ym, xm);
            if(map.at<uchar>(ym, xm) != 255) {
                Intersection newIntersect;
                newIntersect.type = map.at<uchar>(ym, xm);
                newIntersect.distance = dist;
                if(cos(cBeamAngle) > 0) {
                    newIntersect.offset = 1 - ((int)yi - yi);
                } else {
                    newIntersect.offset = ((int)yi - yi);
                }
                ms[i].push_back(newIntersect);
            }
        }

        for(int m = 0; m > -map.rows; m--) {
            float dist = ((float)m - y)/sin(cBeamAngle);
            if(dist < 0)
                continue;

            float yi = m;
            float xi = x + ((float)m-y)*cos(cBeamAngle)/sin(cBeamAngle);
            int xm = (int)xi;
            int ym = - (int)yi;

            if(sin(cBeamAngle) > 0)
                ym--;


            if(xm < 0 || xm >= map.cols || ym < 0 || ym >= map.rows)
                continue;

            //printf("%d %d\n", ym, xm);

            if(map.at<uchar>(ym, xm) != 255) {
                Intersection newIntersect;
                newIntersect.type = map.at<uchar>(ym, xm);
                newIntersect.distance = dist;
                if(sin(cBeamAngle) > 0) {
                    newIntersect.offset = 1 - (xi - (int)xi);
                } else {
                    newIntersect.offset = (xi - (int)xi);
                }
                ms[i].push_back(newIntersect);
            }
        }

        std::sort(ms[i].begin(), ms[i].end(), sorter);

    }

}


double getTime() {
	struct timeval  tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + (double)tv.tv_usec/1000000.0;
}

class Texture {
public:
    enum {
        ANIMATION_STATUS_STOP = 0,
        ANIMATION_STATUS_PLAY_FORWARD,
        ANIMATION_STATUS_PLAY_BACKWARD,
        ANIMATION_STATUS_PAUSE
    };

    Texture() { currentFrame = 0; animationType = 0; prevTime = 0; }
    Texture(cv::Mat _img): image(_img) { currentFrame = 0; animationType = 0; prevTime = 0; }
    cv::Mat image;
    cv::Mat getCol(float offset) {
        int col = offset * image.cols;
        if(col < 0)
            col = 0;
        if(col >= image.cols)
            col = image.cols - 1;

        return image.col(col);
    }

    void addFrame(cv::Mat frame) {
        animation.push_back(frame);
    }


    int animationType; //0 - no animation, 1 - frames, 2 - move down, 3 - move up, 4 - move left, 5 - move right

    double animationTime;

    std::vector<cv::Mat> animation;
    int currentFrame;

    double prevTime;

    int animationStatus;

    void stepAnimation(double currentTime) {

        if(animationStatus == ANIMATION_STATUS_STOP) {
            return;
        } else {
            double diffFramesTime = animationTime / animation.size();
            if(currentTime - prevTime > diffFramesTime) {

                if(animationStatus == ANIMATION_STATUS_PLAY_FORWARD) {
                    currentFrame++;
                    if(currentFrame >= animation.size()) {
                        currentFrame = animation.size() - 1;
                        animationStatus = ANIMATION_STATUS_STOP;
                    }
                    if(currentFrame < animation.size())
                        image = animation[currentFrame];
                } else if(animationStatus == ANIMATION_STATUS_PLAY_BACKWARD) {
                    currentFrame--;
                    if(currentFrame <= 0) {
                        currentFrame = 0;
                        animationStatus = ANIMATION_STATUS_STOP;
                    }
                    if(currentFrame < animation.size())
                        image = animation[currentFrame];
                }
                prevTime = currentTime;
            }
        }
    }

};


class Sprite {
public:
    Sprite() {
        x = 0;
        y = 0;
        currentState = 0;
        width = 1;
        height = 1;
    }

    std::vector<cv::Mat> states;
    int currentState;
    cv::Mat getCurrentMat() {
        if(currentState < states.size()) {
            return states[currentState];
        } else {
            return cv::Mat::zeros(200,200,CV_8UC3);
        }
    }
    float x;
    float y;
    float width;
    float height;

};

typedef std::map<int, Texture> Texpack;

void drawSprites(cv::Mat & render, cv::Mat & zBuffer, std::vector<Sprite> & sprites, float fov, int nBeams, Texpack & texpack, float x, float y, float angle) {
    for(int i = 0; i<sprites.size(); i++) {
        cv::Mat sMat = sprites[i].getCurrentMat();
        float sX = sprites[i].x;
        float sY = sprites[i].y;
        float sDist = sqrt((x-sX)*(x-sX) + (y-sY)*(y-sY));
        float sWidth = sMat.cols*sprites[i].width/sDist;
        float sHeight = sMat.rows*sprites[i].height/sDist;
        float sFi = atan2(sY-y, sX-x);
        float sCent = (float)nBeams/2.0f + (float)nBeams/fov * (angle - sFi);

        printf("sX: %f sY: %f sDist: %f sWidth: %f sHeight: %f sMat.cols: %d sMat.rows: %d sFi: %f sCent: %f\n", sX, sY, sDist, sWidth, sHeight, sMat.cols, sMat.rows, sFi, sCent);

        if((sCent - sWidth/2.0f < nBeams) && (sCent + sWidth/2.0f >= 0)) {
            printf("Draw sprite %d\n", i);
/*
            cv::circle(render, cv::Point(sCent, render.rows/2), render.rows/sDist, cv::Scalar(0,0,255), -1);
            cv::circle(zBuffer, cv::Point(sCent, render.rows/2), render.rows/sDist, cv::Scalar(sDist), -1);
*/
            printf("Cols: ");
            for(int c = std::max(0.0f, sCent - sWidth/2); c < std::min((float)nBeams, sCent + sWidth/2); c++) {

                int col = (float)(c - sCent + sWidth/2)/(float)(sWidth)*(float)sMat.cols;
                if(col < 0) col = 0;
                if(col >= sMat.cols) col = sMat.cols - 1;

                printf("%d<-%d ", c, col);
                cv::Mat spriteLine = sMat.col(col);

                float scaleFactor = 1.0f/sDist;
                //manual scale
                int targetStart = (1.0f - scaleFactor)*render.rows/2.0f;
                int targetEnd = (1.0f + scaleFactor)*render.rows/2.0f;
                for(int r = std::max(targetStart,0); r<std::min(targetEnd,render.rows); r++) {
                    cv::Vec3b colour = spriteLine.at<cv::Vec3b>(spriteLine.rows*(r-targetStart)/(targetEnd-targetStart),0);;
                    if(colour.val[0]==255 && colour.val[1]==255 && colour.val[2]==255) {
                        //transparent
                    } else {
                        if(zBuffer.at<float>(r,c) > sDist) {
                            render.at<cv::Vec3b>(r,c) = colour;
                            zBuffer.at<float>(r,c) = sDist;
                        }
                    }
                }
            }
            printf("\n");


        } else {
            printf("Sprite %d out if fov\n", i);
        }
    }
}

void drawWalls(cv::Mat & render, cv::Mat & zBuffer, cv::Mat & map, float fov, int nBeams, Texpack & texpack, float x, float y, float angle) {
    IntersectionMap ms;
    getAllIntersections(map, x, y, angle, fov, nBeams, ms);


    for(int i = 0; i<ms.size(); i++) {
        for(int d = 0; d<ms[i].size(); d++) {
            int textype = ms[i][d].type;
            float offset = ms[i][d].offset;
            float distance = ms[i][d].distance;
            Texture tex = texpack[textype];
            cv::Mat texLine = tex.getCol(offset);

            float scaleFactor = 1.0f/distance;
            //manual scale
            int targetStart = (1.0f - scaleFactor)*render.rows/2.0f;
            int targetEnd = (1.0f + scaleFactor)*render.rows/2.0f;
            for(int r = std::max(targetStart,0); r<std::min(targetEnd,render.rows); r++) {
                cv::Vec3b colour = texLine.at<cv::Vec3b>(texLine.rows*(r-targetStart)/(targetEnd-targetStart),0);;
                if(colour.val[0]==255 && colour.val[1]==255 && colour.val[2]==255) {
                    //transparent
                } else {
                    if(zBuffer.at<float>(r,i) > distance) {
                        render.at<cv::Vec3b>(r,i) = colour;
                        zBuffer.at<float>(r,i) = distance;
                    }
                }
            }
        }
    }
}


void drawBgSolid(cv::Mat & render) {
    for(int i = 0; i<render.rows; i++) {
        if(i < render.rows/2) {
            int darken = i*100/(render.rows/2);
            cv::line(render, cv::Point(0, i), cv::Point(render.cols-1,i), cv::Scalar(255 - darken,100 - darken,100 - darken));
        } else {
            int darken = (i - render.rows/2)*100/(render.rows/2);
            cv::line(render, cv::Point(0, i), cv::Point(render.cols-1,i), cv::Scalar(100 - darken,100 - darken,100 - darken));
        }
    }
}

void drawBgImage(cv::Mat & render, cv::Mat & bgImage, float yaw, float fov, int nBeams) {

	int bgImageStart = bgImage.cols - yaw*nBeams/fov;
	for(int c = 0; c<render.cols; c++) {
		int cBgImage = bgImageStart + c;
		if(cBgImage >= bgImage.cols) cBgImage -= bgImage.cols;
		bgImage.col(cBgImage).copyTo(render.col(c));
	}

}
void newRenderAt(cv::Mat & render, cv::Mat & map, cv::Mat & bgImage, float fov, int nBeams, Texpack & texpack, std::vector<Sprite> & sprites, float x, float y, float angle) {
    render = cv::Scalar(255,255,255);

    cv::Mat zBuffer = cv::Mat(render.rows, render.cols, CV_32FC1, cv::Scalar(10000));

    drawBgSolid(render);
    //drawBgImage(render, bgImage, angle, fov, nBeams);
    //drawSprites(render, zBuffer, sprites, fov, nBeams, texpack, x, y, angle);
    drawWalls(render, zBuffer, map, fov, nBeams, texpack, x, y, angle);
}


void calcNewPosition(cv::Mat & map, float & x, float & y, float & angle, float df, float ds, float dangle, float dt) {
    float dx = df * cos(angle) + ds * sin(angle);
    float dy = df * sin(angle) - ds * cos(angle);
    if(dt == 0) {
        return;
    }

//    if(map.at<uchar>(y + dy * dt, x + dx * dt) == 255) {
        x += dx * dt;
        y += dy * dt;
//    }
    angle += dangle * dt;

    while(angle < -M_PI) angle += 2.0f*M_PI;
    while(angle > M_PI) angle -= 2.0f*M_PI;



}

void moveSprites(std::vector<Sprite> & sprites) {
    sprites[0].x = 32.0f + 3.0f*sin(getTime()/10.0f);
    sprites[0].y = -32.0f + 3.0f*cos(getTime()/10.0f);
}

void animateTextures(Texpack & texpack, double cTime) {
    Texpack::iterator it = texpack.begin();
    for(; it!=texpack.end(); it++) {
        (*it).second.stepAnimation(cTime);
    }
}

int main() {


	cv::namedWindow("render", cv::WINDOW_NORMAL);

	cv::Mat map = cv::imread("test_map.bmp", cv::IMREAD_GRAYSCALE);

	int wndWidth = 1440;
	int wndHeight = 900;

	cv::Mat render(wndHeight,wndWidth, CV_8UC3);

	float df = 0.0f;
	float ds = 0.0f;
	float x = 30.0f;
    float y = -20.0f;
	float angle = -M_PI/2.0f;
	float dangle = 0.3f;
	float fov = M_PI/3.0f;

    std::vector<Sprite> sprites;

    Sprite ballSprite;
//    ballSprite.states.push_back(cv::imread("svidetel.jpg"));
//    ballSprite.x = 32;
//    ballSprite.y = -32;
//    ballSprite.width = 2;
    sprites.push_back(ballSprite);

    Texpack texpack;
    texpack[50] = Texture(cv::imread("wall_window.bmp"));
    texpack[100] = Texture(cv::imread("wood_wall.jpeg"));
    texpack[150] = Texture(cv::imread("brick_wall.jpeg"));
    texpack[200] = Texture(cv::imread("doom_door.jpeg"));

    for(int i = 1; i<15; i++) {
        char buf[1000];
        sprintf(buf,"doom_door_%d.jpg", i);
        texpack[200].addFrame(cv::imread(buf));
    }
    for(int i = 14; i>0; i--) {
        char buf[1000];
        sprintf(buf,"doom_door_%d.jpg", i);
        texpack[200].addFrame(cv::imread(buf));
    }

    texpack[200].animationTime = 2;

	cv::Mat bgImageRaw = cv::imread("bg.png");
	cv::Mat bgImage;
	cv::resize(bgImageRaw, bgImage, cv::Size(2.0f*M_PI*(float)wndWidth/fov,wndHeight));

	double prevTime = getTime();
	while(1) {

        double cTime = getTime();

        animateTextures(texpack, cTime);

        calcNewPosition(map, x, y, angle, df, ds, dangle, cTime - prevTime);

        moveSprites(sprites);

        prevTime = cTime;

        newRenderAt(render, map, bgImage, fov, wndWidth, texpack, sprites, x, y, angle);
		cv::imshow("render",render);


        cv::Mat mapD;
        cv::resize(map, mapD, cv::Size(640,640), 0, 0, cv::INTER_NEAREST);
        cv::line(mapD, cv::Point(x*10.0f,-y*10.0f), cv::Point(x + cos(-angle - fov/2.0f)*1000, y + sin(-angle - fov/2.0f)*1000)*10.0f, cv::Scalar(0,255,0,0),1);
        cv::line(mapD, cv::Point(x*10.0f,-y*10.0f), cv::Point(x + cos(-angle + fov/2.0f)*1000, y + sin(-angle + fov/2.0f)*1000)*10.0f, cv::Scalar(0,255,0,0),1);
        cv::imshow("map", mapD);


        int k = cv::waitKey(1);

		if((char)k == 'q') {
			break;
		} else if((char)k == ',') {
            ds = 5.0f;
		} else if((char)k == '.') {
            ds = -5.0f;
		} else if((char)k == 'a') {
            dangle = +1.0f;
		} else if((char)k == 'd') {
            dangle = -1.0f;
		} else if((char)k == 's') {
            df = -5.0f;
		} else if((char)k == 'w') {
            df = 5.0f;
		} else if((char)k == '[') {
			fov -= M_PI/180.0f;
            //myLidar = VirtualLidar(fov, render.cols, 0);
		} else if((char)k == ']') {
			fov += M_PI/180.0f;
            //myLidar = VirtualLidar(fov, render.cols, 0);
		} else if((char)k == 'e'){
			ds = 0;
			df = 0;
			dangle = 0;
		} else {

            if(df > 1)
                df -= 5;
            else if(df < -1)
                df += 5;
            else
                df = 0;

            if(ds > 1)
                ds -= 5;
            else if(ds < -1)
                ds += 5;
            else
                ds = 0;

            if(dangle > 0.5f) {
                dangle -= 1.0f;
            } else if(dangle < -0.5f) {
                dangle += 1.0f;
            } else {
                dangle = 0;
            }
		}

	}
	return 0;
}
