#include <string>

#include "format.h"

using std::string;
using std::to_string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::AddZero(const long tt){
    if (tt<10){return "0"+to_string(tt);}
    else return to_string(tt);
}
string Format::ElapsedTime(long seconds) {
    long time =seconds;
    long hrs, min,sec;
    hrs=time/3600;
    time = time%3600;
    min = time/60;
    time = time%60;
    sec = time;
    return AddZero(hrs) + ":" + AddZero(min)+ ":" + AddZero(sec);
 }