#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include "NtKinect.h"
using namespace std;
#include <time.h>
#include <cmath>


#include <fstream>


//���� �ð� ����
string now() {
    char s[1024];
    time_t t = time(NULL);
    struct tm lnow;
    localtime_s(&lnow, &t);
    sprintf_s(s, "%04d-%02d-%02d_%02d-%02d-%02d", lnow.tm_year + 1900, lnow.tm_mon + 1, lnow.tm_mday, lnow.tm_hour, lnow.tm_min, lnow.tm_sec);
    return string(s);
}

//���α׷� ����
void doJob() {
    NtKinect kinect;

    std::string name1;
    std::string name2;
    std::string name3;


    //�������� 
    int fram = 0;    // ������
    float rate_start_Y, rate_end_Y = 0.0;    // Y ��ȭ�� ���� ����

    float rate_start_X, rate_end_X = 0.0;    // X ��ȭ�� ���� ����
    float rate_start_Z, rate_end_Z = 0.0;    // Z ��ȭ�� ���� ����

    float coor_Y = 0.0;    // �Ӹ��� Y��ǥ
    float coor_X = 0.0;    // �Ӹ��� X��ǥ
    float coor_Z = 0.0;    // �Ӹ��� Z��ǥ
    char key_save = 'r';    // ���� �˰������� ���� key
    int dang = -2;    // ������� �Ͻ� �����ϱ� ���� ����
    int emer = 0;
    float coor_XYZ = 0.0;   // 3�����󿡼��� �̵��Ÿ�
    float list_daily[10000] = { 0.0, };
    float list_emer[1000] = { 0.0, };
    int dail_value = 0;
    int emer_value = 0;
    char txt[5][100] = { {"\n"}, };

    //������ ȯ�漳��
    cv::VideoWriter vw;
    int scale = 1;
    cv::Size sz(1920 / scale, 1080 / scale);
    bool onSave = false;
    cv::Mat img;

    clock_t start, end;
    float result;


    while (1) {
        kinect.setRGB();
        kinect.setSkeleton();
        for (auto person : kinect.skeleton) {
            for (auto joint : person) {
                if (joint.TrackingState == TrackingState_NotTracked) continue;
                ColorSpacePoint cp;
                if (joint.JointType == 3) {
                    std::string s_result_Y = "Y coordinate of Head : ";
                    std::string s_result_X = "X coordinate of Head : ";
                    std::string s_result_Z = "Z coordinate of Head : ";
                    coor_Y = joint.Position.Y;
                    coor_X = joint.Position.X;
                    coor_Z = joint.Position.Z;
                    string s_coor_Y = std::to_string(coor_Y);
                    string s_coor_X = std::to_string(coor_X);
                    string s_coor_Z = std::to_string(coor_Z);
                    s_result_Y = s_result_Y + s_coor_Y;
                    s_result_X = s_result_X + s_coor_X;
                    s_result_Z = s_result_Z + s_coor_Z;
                    cv::putText(kinect.rgbImage, s_result_Y, cv::Point(30, 30), cv::FONT_HERSHEY_PLAIN, 2.0, cv::Scalar(255, 0, 0), 2);
                    cv::putText(kinect.rgbImage, s_result_X, cv::Point(30, 60), cv::FONT_HERSHEY_PLAIN, 2.0, cv::Scalar(255, 0, 0), 2);
                    cv::putText(kinect.rgbImage, s_result_Z, cv::Point(30, 90), cv::FONT_HERSHEY_PLAIN, 2.0, cv::Scalar(255, 0, 0), 2);
                }
                kinect.coordinateMapper->MapCameraPointToColorSpace(joint.Position, &cp);
                cv::rectangle(kinect.rgbImage, cv::Rect((int)cp.X - 5, (int)cp.Y - 5, 10, 10), cv::Scalar(0, 0, 255), 2);
            }
        }
        if (emer > 0) {
            cv::putText(kinect.rgbImage, "EMERGENCY", cv::Point(550, 150), cv::FONT_HERSHEY_PLAIN, 4.0, cv::Scalar(0, 0, 255), 2);
            emer = emer + 1;
        }
        if (emer == 100) {
            emer = 0;
        }
        if (onSave) {
            cv::resize(kinect.rgbImage, img, sz, 0, 0);
            cv::cvtColor(img, img, CV_BGRA2BGR);
            vw << img;
        }

        if (fram == 0) {

            start = clock();

            //���� ����
            name3 = now();
            //std::cout << name3 << "\n";
            vw = cv::VideoWriter(name3 + ".avi", CV_FOURCC_MACRO('X', 'V', 'I', 'D'), 10.0, sz);
            //vw = cv::VideoWriter(now()+".mp4",CV_FOURCC_MACRO('F','M','P','4'), 30.0, sz);
            //vw = cv::VideoWriter(now()+".mp4",CV_FOURCC_MACRO('M','P','4','V'), 30.0, sz);
            if (!vw.isOpened()) throw runtime_error("cannot create video file");
            onSave = true;

            rate_start_Y = coor_Y;
            rate_start_X = coor_X;
            rate_start_Z = coor_Z;
        }
        else if (fram % 4 == 0) { //0.1�� = 1fram   //10������ = 1���� //  Y ��ȭ�� ����
            end = clock();
            result = (end - start) / 1000.0;
            //printf("%f\n", result);

            rate_end_Y = coor_Y;
            rate_end_X = coor_X;
            rate_end_Z = coor_Z;

            coor_XYZ = sqrt(((rate_start_X - rate_end_X) * (rate_start_X - rate_end_X)) +
                ((rate_start_Y - rate_end_Y) * (rate_start_Y - rate_end_Y)) +
                ((rate_start_Z - rate_end_Z) * (rate_start_Z - rate_end_Z)));

            rate_start_Y = rate_end_Y;
            rate_start_X = rate_end_X;
            rate_start_Z = rate_end_Z;

            if ((coor_XYZ > 0.59) && (coor_XYZ < 1.0)) {    //���޻�Ȳ ���� //������ ���� ���� ���� �ʿ�ڡڡڡڡڡڡڡڡڡ�
                dang += 1;
                emer = emer + 1;
                list_emer[emer_value] = coor_XYZ; //���޻�Ȳ �� list_emer�� �߰�
                emer_value = emer_value + 1;
            }
          
        }
        fram = fram + 1;

        if (fram > 100) { //10�ʸ��� ���� ����(�����ڵ�) : ���� ���� ����
            key_save = 's';
            if (key_save == 's' && onSave) {
                vw.release();
                onSave = false;
            }
            fram = 0;
            


            if (dang > 0) {
                //�������� ���� ���߱� �ڵ�
                dang++;
                if (dang == 4) {
                    //���Ⱑ ���޵� 2��° txt�κ�����
                    string str = name3 + "\n";
                    strcpy(txt[4], str.c_str());

                    FILE* fp = fopen("emergence.txt", "w+");
                    for (int i = 0; i < 5; i++) {
                        fprintf(fp, "%s", txt[i]);
                    }
                    fclose(fp);


                    dang = -2;
                }
                else if (dang == 2) {//���Ⱑ ���޻�Ȳ �߻��� ó��, ���޻�Ȳ, ��1, ��2 ��° �� 3�� txt�� ������ڡڡڡڡڡڡڡڡ�3�� ���ÿ�???
                    string str1 = name1 + "\n";
                    strcpy(txt[0], str1.c_str());
                    string str2 = name2 + "\n";
                    strcpy(txt[1], str2.c_str());
                    string str3 = name3 + "\n";
                    strcpy(txt[2], str3.c_str());
                }
                else if (dang == 3) {//���Ⱑ ���޵� 1��° txt�� ������
                    string str = name3 + "\n";
                    strcpy(txt[3], str.c_str());
                }
            }
           
            else if (dang == 0) { //dang == 0

                 char strPath[] = { "C:\\Users\\�ӽ�ȣ\\source\\repos\\opencv\\opencv\\" };
                 char path[1000];
                 std::string str1(strPath);
                 
                 str1 = str1 + name1+".avi";
                 strcpy(path,str1.c_str());
                 
                 
                 int dele = remove(path);

                 name1 = name2;
                 name2 = name3;

                 if (dele == 0) {
                     //printf("���ϻ��� ����\n");
                 }
                 else if (dele == -1) {
                     //printf("���ϻ��� ����\n");
                 }

            }

            else if (dang == -1) {
                name1 = name2;
                name2 = name3;
                dang = dang + 1;
            }

            else if (dang == -2) {
                name2 = name3;
                //std::cout << "name 3 : " << name3 << "\n";
               // std::cout << "name 2 : " << name2 << "\n";
                dang = dang + 1;
            }
        }


        //�Կ� ȭ�� ���
        cv::imshow("rgb", kinect.rgbImage);
        auto key = cv::waitKey(33);
        if (key == 'q') break;
    }
    for (int i = 0; i < 5; i++) {
        printf("%s", txt[i]);
    }
    cv::destroyAllWindows();
}

int main(int argc, char** argv) {
    try {
        doJob();
    }
    catch (exception& ex) {
        cout << ex.what() << endl;
        string s;
        cin >> s;
    }
    return 0;
}
