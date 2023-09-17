#include "../def.hpp"
/*
cl /EHsc /std:c++20 log.cpp /I"C:/_repositories/ws_vscode/ws_cpp_library/mcc_library/" /Fe: m.exe

/I"C:\_github\wsVinylPeeler\remote_api\vs2022mfcDlg_VPRemote\VPRemote\" /I"C:\_github\wsVinylPeeler\remote_api\vs2022mfcDlg_VPRemote\VPRemote\\framework\" /I"C:\_github\wsVinylPeeler\remote_api\vs2022mfcDlg_VPRemote\VPRemote\\framework\common\" /I"C:\_github\wsVinylPeeler\remote_api\vs2022mfcDlg_VPRemote\VPRemote\\framework\hal\" /I"C:\_github\wsVinylPeeler\remote_api\vs2022mfcDlg_VPRemote\VPRemote\\framework\hal\_inc\"

*/

template<typename = int>
void LogInfo(const char* str, ...)
{
    if (str)
    {
        va_list args;
        va_start(args, str);

        vfprintf(stdout, str, args);

        va_end(args);
    }
}

template<typename = int>
void LogError(const char* str, ...)
{
    if (str)
    {
        va_list args;
        va_start(args, str);

        vfprintf(stderr, str, args);

        va_end(args);
    }
}


int main ()
{
    using namespace std;

    const char* filename = "test file";
    LogInfo("log info: timer test start '%s'.\n", filename);

     vector<string> buffer; // 입력된 데이터를 저장할 버퍼 리스트
    int cur_pos = -1; // 현재 입력중인 데이터의 인덱스 (-1로 초기화)
    string cur_str; // 현재 입력중인 문자열
    uint32_t pre_ms = tim::millis();
    int cnt = 0;
    while (true)
    {

        if (tim::millis() - pre_ms > 500)
        {
            ++cnt;
           
            cerr << "\033[34m" <<  tim::NowStr()<<"\t"<<pre_ms << "\033[0m " << endl;
            pre_ms = tim::millis();
        }

        if (cnt > 10)
            break;

       // mcc::tim::delay(10);




#if 0
        else if (c == 13) { // Enter 키 입력 시 현재 입력중인 데이터를 버퍼 리스트에 추가하고 출력
            buffer.insert(buffer.begin(), cur_str); // 현재 입력중인 문자열을 버퍼 리스트의 가장 앞쪽에 추가
            cur_str.clear(); // 현재 입력중인 문자열 초기화
            cur_pos = -1; // 현재 입력중인 데이터의 인덱스도 초기화
            cout << endl;
            for (int i = 0; i < buffer.size(); i++) {
                // space로 구분된 문자열 분리해서 출력
                string str = buffer[i];
                string delim = " ";
                size_t pos = 0;
                string token;
                int cnt = i + 1;
                cout << cnt << ":";
                while ((pos = str.find(delim)) != string::npos) {
                    token = str.substr(0, pos);
                    str.erase(0, pos + delim.length());
                    if (isdigit(token[0])) {
                        // 숫자인 경우 흰색으로 출력
                        cout << "\033[37m" << token << "\033[0m ";
                    }
                    else {
                        // 문자인 경우 파란색으로 출력
                        cout << "\033[34m" << token << "\033[0m ";
                    }
                }
                if (isdigit(str[0])) {
                    // 숫자인 경우 흰색으로 출력
                    cout << "\033[37m" << str << "\033[0m" << endl;
                }
                else {
                    // 문자인 경우 파란색으로 출력
                     cout << "\033[34m" << str << "\033[0m ";
                }
            }
        }
        #endif
    }
    return 0;
}