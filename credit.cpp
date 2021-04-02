#include <iomanip>
#include <cmath>
#include "credit.h"

void Credit::ResetAll(){
    // стирает весь объект
    _dates.reset();
    _matrix.reset();
}
void Credit::ClearMatrixVectors(){
    // обнуляет все векторы матрицы
    if(_matrix){
        for(int i=0;i<dates_count;i++)
            _matrix[i].clear();
    }
}
void Credit::BuildAll(const int m){
    // выделяет память под данные
    if(m >= MIN_MONTHS && m <=MAX_MONTHS){
        ResetAll();
        months_ = m;
        dates_count = months_ + 1;
    }else{
        std::ostringstream _ex("");
        _ex << "ERROR: Wrong count of months";
        throw _ex.str();
    }
    try{
        _dates = std::unique_ptr<QDate[]>(new QDate[dates_count]);
    }catch(...){
        std::ostringstream _ex("");
        _ex << "ERROR: bad_alloc in building dates";
        throw _ex.str();
    }
    try{
        _matrix = std::unique_ptr<std::vector<double>[]>(new std::vector<double> [dates_count]);
    }catch(...){
        _dates.reset();
        std::ostringstream _ex("");
        _ex << "ERROR: bad_alloc in building vectors";
        throw _ex.str();
    }
}
double Credit::getMonthRatio(const QDate& date1, const QDate& date2){
    // вычисляет отношение числа дней между датами "date1" и "date2" к количеству дней в году
    int _year1,_year2;
    _year1 = date1.year();
    _year2 = date2.year();
    int i;
    double result=0;
    i=_year2-_year1;
    if(i == 0)
        return (double)date1.daysTo(date2)/(double)date1.daysInYear();
    else if(i>=2)
        result = (i-1);
    else if(i<0)
        return 0.0;
    result += (double)date1.daysTo(QDate(_year1,12,31))/(double)date1.daysInYear();
    result += (double)QDate(_year2,01,01).daysTo(date2)/(double)date2.daysInYear();
    return result;
}
void Credit::CalculateDates(const QDate& Date, const int m){
    // строит даты выплат на "m" месяцев, начиная с даты "Date"
    try{
        BuildAll(m);
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
void Credit::CalculateCredit(const double Debt, const double Percent, const bool Cents, const double Def_payment){
    // ГЛАВНАЯ ФУНКЦИЯ - РАСЧЁТ КРЕДИТА
    if(Debt<1.0){
        std::ostringstream _ex("");
        _ex << "ERROR: Debt is less then 1.0";
        throw _ex.str();
    }
    if(Percent <= 0.0 || Percent >= 100.0){
        std::ostringstream _ex("");
        _ex << "ERROR: Wrong percent";
        throw _ex.str();
    }
    if(Def_payment >= Debt){
        std::ostringstream _ex("");
        _ex << "ERROR: Def_payment is equal or more then Debt";
        throw _ex.str();
    }
    if(!_dates){
        std::ostringstream _ex("");
        _ex << "ERROR: Dates not initialized";
        throw _ex.str();
    }

    ClearMatrixVectors();

    double percent_sum=0.0, debt_sum=0.0, total_sum=0.0;
    double Ostatok,month_percent;
    double preDebt,preX;
    double tmp;
    int i;

    debt_ = Debt;
    year_percent_ = Percent/100.0;
    if(Def_payment>0)
        vznos_ = Def_payment;
    else{
        preDebt = 1.0 + year_percent_*getMonthRatio(_dates[0],_dates[1]);
        preX = 1.0;
        for(i=1;i<months_;i++){
            tmp = 1.0 + year_percent_*getMonthRatio(_dates[i],_dates[i+1]);
            preDebt *= tmp;
            preX = 1.0 + preX*tmp;
        }
        vznos_ = Debt*preDebt/preX;
        if(Cents)
            vznos_ = (double)ceil(100.0*Debt*preDebt/preX)/100.0;
        else
            vznos_ = (double)ceil(Debt*preDebt/preX);
    }

    Ostatok = Debt;
    for(i=0;i<months_-1;i++){
        month_percent = year_percent_*getMonthRatio(_dates[i],_dates[i+1]);
        _matrix[i].push_back((double)ceil(100.0*Ostatok*month_percent)/100.0);
        tmp = vznos_ - _matrix[i][0];
        Ostatok -= tmp;
        _matrix[i].push_back(tmp);
        _matrix[i].push_back(vznos_);
        _matrix[i].push_back(Ostatok);
        percent_sum += _matrix[i][0];
        debt_sum += _matrix[i][1];
        total_sum += _matrix[i][2];
    }
    month_percent = year_percent_*getMonthRatio(_dates[i],_dates[i+1]);
    _matrix[i].push_back((double)ceil(100.0*Ostatok*month_percent)/100.0);
    _matrix[i].push_back(Ostatok);
    tmp = Ostatok + _matrix[i][0];
    _matrix[i].push_back(tmp);
    _matrix[i].push_back(0);

    _matrix[i+1].push_back(percent_sum + _matrix[i][0]);
    _matrix[i+1].push_back(debt_sum + _matrix[i][1]);
    _matrix[i+1].push_back(total_sum + _matrix[i][2]);

    if(tmp > vznos_ && Def_payment == 0){
        try{
            if(Cents)
                CalculateCredit(Debt,Percent,Cents,vznos_+0.01);
            else
                CalculateCredit(Debt,Percent,Cents,vznos_+1.0);
        }catch(...){
            throw;
        }
    }
}
void Credit::IncDate(const int date_no){
    // увеличивает дату под номером "date_no" на один день
    if(!_dates)
        return;
    if(date_no < 0 || date_no >= dates_count)
        return;
    QDate tmp_date = _dates[date_no].addDays(1);
    if(date_no == dates_count-1)
        _dates[date_no] = tmp_date;
    else if(tmp_date != _dates[date_no+1])
        _dates[date_no] = tmp_date;
}
void Credit::DecDate(const int date_no){
    // уменьшает дату под номером "date_no" на один день
    if(!_dates)
        return;
    if(date_no < 0 || date_no >= dates_count)
        return;
    QDate tmp_date = _dates[date_no].addDays(-1);
    if(date_no == 0)
        _dates[date_no] = tmp_date;
    else if(tmp_date != _dates[date_no-1])
        _dates[date_no] = tmp_date;
}
std::shared_ptr<QDate[]> Credit::GetDates() const{
    // возвращает указатель на рассчитанные даты
    return _dates;
}
std::shared_ptr<std::vector<double>[]> Credit::GetMatrix() const{
    // возвращает указатель на рассчитанные данные
    return _matrix;
}
double Credit::GetDebt() const{
    // возвращает запрошенную сумму
    return debt_;
}
double Credit::GetPercent() const{
    // возвращает годовой процент (дробный)
    return year_percent_;
}
int Credit::GetMonths() const{
    // возвращает количество месяцев
    return months_;
}
int Credit::GetMonthPayment() const{
    // возвращает сумму ежемесячного платежа
    return vznos_;
}
