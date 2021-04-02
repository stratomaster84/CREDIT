#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <cmath>
#include <QDate>

using namespace std;

#define MAX_MONTHS 120
#define MIN_MONTHS 3

class Credit{
    unique_ptr<QDate[]> _dates;
    unique_ptr<vector<string>[]> _matrix;

    int months_ = 0;
    int dates_count = 0;
    int rows_count = 0;

public:
    Credit(){}
    Credit(const Credit&) = delete;
    Credit operator=(const Credit&) = delete;
    void ClearAll(){
        _dates.release();
        _matrix.release();
        months_ = 0;
        dates_count = 0;
        rows_count = 0;
    }
    void ClearMatrix(){
        for(int i=1;i<rows_count;i++)
            _matrix[i].clear();
    }
    void Build(const int& m){
        if(m >= MIN_MONTHS && m <=MAX_MONTHS){
            ClearAll();
            months_ = m;
            dates_count = months_ + 1;
            rows_count = months_ + 2;
        }else{
            std::ostringstream _ex("");
            _ex << "ERROR: Wrong count of months";
            throw _ex.str();
        }
        _dates = unique_ptr<QDate[]>(new QDate[dates_count]);
        _matrix = unique_ptr<vector<string>[]>(new vector<string> [rows_count]);
        _matrix[0].push_back("Date");
        _matrix[0].push_back("Percent Payment");
        _matrix[0].push_back("Debt Payment");
        _matrix[0].push_back("Sum Payment");
        _matrix[0].push_back("Debt balance after pay");
    }
    void GetDates(const QDate& Date, const int& m){
        try{
            Build(m);
        }catch(...){
            throw;
        }
        if(!Date.isValid()){
            std::ostringstream _ex("");
            _ex << "ERROR: Wrong date";
            throw _ex.str();
        }
        QDate CurDate = Date;
        _dates[0] =  CurDate;
        for(int i=1;i<=months_;i++){
            int _days = CurDate.daysInMonth();
            _dates[i] = _dates[i-1].addDays(_days);
            CurDate = CurDate.addMonths(1);
        }
    }
    double _mproc(QDate date1, QDate date2){
        unsigned short _year1,_year2;
        _year1 = date1.year();
        _year2 = date2.year();
        int i;
        double _tmp=0;
        i=_year2-_year1;
        if(i == 0)
            return (double)date1.daysTo(date2)/(double)date1.daysInYear();
        else if(i>=2)
            _tmp = (i-1);
        else if(i<0)
            return 0.0;
        _tmp += (double)date1.daysTo(QDate(_year1,12,31))/(double)date1.daysInYear();
        _tmp += (double)QDate(_year2,01,01).daysTo(date2)/(double)date2.daysInYear();
        return _tmp;
    }
    void Print(){
        if(_matrix){
            for(int i=0;i<rows_count;i++){
                cout << _matrix[i][0] << '\t' << _matrix[i][1] << '\t' << _matrix[i][2]
                                      << '\t' << _matrix[i][3] << '\t' << _matrix[i][4] << '\n';
            }
        }
    }
    void Calculate(const double& Debt, const double& Percent, const bool& Cents = true, const int& Def_payment = 0)
    {
        if(!_dates){
            std::ostringstream _ex("");
            _ex << "ERROR: Dates not initialized";
            throw _ex.str();
        }

        ClearMatrix();

        double _Summa = Debt;
        double _proc = Percent/100.0;
        const unsigned short _prec = 100;
        double _tmp1,_tmp2,_Vznos;
        double _Ostatok,_monthproc;

        _tmp1 = _Summa*(1 + _proc*_mproc(_dates[0],_dates[months_]))/months_;
        _tmp2 = 0;
        int i,j;
        for(j=0;j<_prec;j++){
            _Vznos = (_tmp2 + _tmp1)/2.0;
            _Ostatok  = _Summa;
            for(i=0;i<months_;i++){
                _monthproc = _proc*_mproc(_dates[i],_dates[i+1]);
                _Ostatok = _Ostatok*(1+_monthproc) - _Vznos;
            }
            if(_Ostatok == 0)
                j = _prec;
            else if(_Ostatok < 0)
                _tmp1 = _Vznos;
            else
                _tmp2 = _Vznos;
        }
        if(Cents)
            _Vznos = (double)ceil(100.0*(_tmp2>_tmp1?_tmp2:_tmp1))/100.0;
        else
            _Vznos = (double)ceil((_tmp2>_tmp1?_tmp2:_tmp1));
        if(Def_payment>0)
            _Vznos = Def_payment;

        _Ostatok = _Summa;
        double _sumProc=0;
        for(i=1;i<months_;i++){
            _matrix[i].push_back(to_string(_dates[i].day()) + "." + to_string(_dates[i].month())
                                 + "." + to_string(_dates[i].year()));
            _monthproc = _proc*_mproc(_dates[i-1],_dates[i]);
            _tmp1 =  (double)ceil(_Ostatok*_monthproc*100.0)/100.0;
            _sumProc += _tmp1;
            _matrix[i].push_back(to_string(_tmp1));
            _matrix[i].push_back(to_string((double)ceil(100.0*(_Vznos - _tmp1))/100.0));
            _matrix[i].push_back(to_string(_Vznos));
            _Ostatok -= _Vznos - _tmp1;
            _matrix[i].push_back(to_string((double)ceil(100.0*(_Ostatok))/100.0));
        }

        _matrix[i].push_back(to_string(_dates[i].day()) + "." + to_string(_dates[i].month())
                             + "." + to_string(_dates[i].year()));
        _monthproc = _proc*_mproc(_dates[i-1],_dates[i]);
        _tmp1 =  (double)ceil(_Ostatok*_monthproc*100.0)/100.0;
        _sumProc += _tmp1;
        _matrix[i].push_back(to_string(_tmp1));
        _matrix[i].push_back(to_string((double)ceil(100.0*(_Ostatok))/100.0));
        _matrix[i].push_back(to_string((double)ceil(100.0*(_Ostatok+_tmp1))/100.0));
        _matrix[i].push_back("0");

        i++;
        _matrix[i].push_back("Total");
        _matrix[i].push_back(to_string((double)ceil(100.0*(_sumProc))/100.0));
        _matrix[i].push_back(to_string((double)ceil(100.0*(_Summa))/100.0));
        _matrix[i].push_back(to_string((double)ceil(100.0*(_sumProc+_Summa))/100.0));
        _matrix[i].push_back("");
    }
    void IncDate(const int& date_no){
        if(date_no < 0 || date_no >= dates_count)
            return;
        QDate tmp_date = _dates[date_no].addDays(1);
        if(date_no == dates_count-1)
            _dates[date_no] = tmp_date;
        else if(tmp_date != _dates[date_no+1])
            _dates[date_no] = tmp_date;
    }
    void DecDate(const int& date_no){
        if(date_no < 0 || date_no >= dates_count)
            return;
        QDate tmp_date = _dates[date_no].addDays(-1);
        if(date_no == 0)
            _dates[date_no] = tmp_date;
        else if(tmp_date != _dates[date_no-1])
            _dates[date_no] = tmp_date;
    }
};

int main(){
    Credit _mine;

    QDate date(2017,02,11);
    int months = 60;
    float debt = 506329;
    float percent = 17;

    _mine.GetDates(date,months);
    _mine.IncDate(1);
    _mine.IncDate(1);
    _mine.IncDate(4);
    _mine.IncDate(4);
    _mine.IncDate(9);
    _mine.IncDate(9);
    _mine.IncDate(12);
    _mine.IncDate(13);
    _mine.IncDate(18);
    _mine.IncDate(18);
    _mine.IncDate(21);
    _mine.IncDate(27);
    _mine.IncDate(27);
    _mine.IncDate(30);
    _mine.IncDate(35);
    _mine.IncDate(35);
    _mine.IncDate(38);
    _mine.IncDate(38);
    _mine.IncDate(41);
    _mine.IncDate(41);
    _mine.IncDate(44);
    _mine.IncDate(50);
    _mine.IncDate(53);
    _mine.IncDate(55);
    _mine.IncDate(55);
    _mine.IncDate(58);
    _mine.IncDate(58);
    _mine.Calculate(debt,percent,true);
    _mine.Print();

    return 0;
}







