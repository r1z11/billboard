#ifndef HELPERFUNCS
#define HELPERFUNCS

#include <QMessageBox>
#include <QDebug>

/*
    helper functions
*/

class HelperFunctions {
private:
    double nhrs, nmins;

public:

    //get billable time
    void getTime(QString time)
    {
        QString tmpstr;
        int j=0;

        for(int i=0; i<5; i++){
            if(time[i].isNumber()){
                j = i;

                if(i<1){
                    tmpstr = time[j] % time[j+1];
                    nhrs = tmpstr.toDouble();
                }else{
                    if(i==3){
                        tmpstr = time[j] % time[j+1];
                        nmins = tmpstr.toDouble();
                    }
                }
            }
        }
    }

    //get hours
    double getHrs()
    {
        int hrs = nhrs;
        return hrs;
    }

    //get minutes
    double getMins()
    {
        int mins = nmins;
        return mins;
    }

};

#endif // HELPERFUNCS

