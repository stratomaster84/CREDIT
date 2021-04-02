#include <iomanip>
#include <cmath>
#include "credit.h"

//==================  Стирает весь объект  =======================
void Credit::ResetAll(){
    if(_dates){
        delete [] _dates;
        _dates = nullptr;
    }
    if(_matrix){
        ClearMatrixVectors();
        delete [] _matrix;
        _matrix = nullptr;
    }
}
//==================  Обнуляет все векторы матрицы  =======================
void Credit::ClearMatrixVectors(){
    if(_matrix){
        for(int i=0;i<dates_count;i++)
            _matrix[i].clear();
    }
}
//==================  Выделяет память под данные  =======================
void Credit::BuildAll(const int m){
    if(m >= MIN_MONTHS && m <=MAX_MONTHS){
        ResetAll();
        months_ = m;
        dates_count = months_ + 1;
    }else{
        throw std::string("ОШИБКА: неверное количество месяцев");
    }
    try{
        _dates = new QDate[dates_count];
    }catch(...){
        _dates = nullptr;
        throw std::string("ERROR: bad_alloc while building dates");
    }
    try{
        _matrix = new std::vector<double> [dates_count];
    }catch(...){
        delete [] _dates;
        _dates = nullptr;
        _matrix = nullptr;
        throw std::string("ERROR: bad_alloc while building vectors");
    }
}
//====  Вычисляет отношение числа дней между датами "date1" и "date2" к количеству дней в году  ====
double Credit::getMonthRatio(const QDate& date1, const QDate& date2){
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
//=====  Строит даты выплат на "m" месяцев, начиная с даты "Date"  =====
void Credit::CalculateDates(const QDate& Date, const int m){
    if(!Date.isValid()){
        throw std::string("ОШИБКА: Неверная дата");
    }
    try{
        BuildAll(m);
    }catch(...){
        throw;
    }
    QDate CurDate = Date;
    _dates[0] =  CurDate;
    for(int i=1;i<=months_;i++){
        int _days = CurDate.daysInMonth();
        _dates[i] = _dates[i-1].addDays(_days);
        CurDate = CurDate.addMonths(1);
    }
}
//==================  ГЛАВНАЯ ФУНКЦИЯ - РАСЧЁТ КРЕДИТА  =======================
void Credit::CalculateCredit(const double Debt, const double Percent, const bool Cents, const double Def_payment){
    if(Debt<1.0){
        throw std::string("ОШИБКА: Запрошенная сумма меньше 1.00");
    }
    if(Percent <= 0.0 || Percent >= 100.0){
        throw std::string("ОШИБКА: Неверный годовой процент");
    }
    if(Def_payment >= Debt){
        throw std::string("ОШИБКА: Взнос по-умолчанию меньше или\nравен запрошенной сумме");
    }
    if(!_dates){
        throw std::string("ОШИБКА: Даты не инициализированы");
    }

    ClearMatrixVectors();

    double percent_sum=0.0, debt_sum=0.0, total_sum=0.0;    // переменные для создания последней строки
    double Ostatok,month_percent;
    double preDebt,preX;
    double tmp;
    int i;

    debt_ = Debt;
    year_percent_ = Percent/100.0;
    if(Def_payment>0)
        vznos_ = Def_payment;   // если есть, установить взнос по-умолчанию
    else{
        // расчёт взноса по вычисленной формуле
        preDebt = 1.0 + year_percent_*getMonthRatio(_dates[0],_dates[1]);
        preX = 1.0;
        for(i=1;i<months_;i++){
            tmp = 1.0 + year_percent_*getMonthRatio(_dates[i],_dates[i+1]);
            preDebt *= tmp;
            preX = 1.0 + preX*tmp;
        }
        vznos_ = Debt*preDebt/preX;
        if(Cents)
            vznos_ = (double)ceil(100.0*Debt*preDebt/preX)/100.0;   // если взнос - дробный
        else
            vznos_ = (double)ceil(Debt*preDebt/preX);               // если взнос целочисленный
    }

    Ostatok = Debt;
    // расчёт строк, кроме двух последних
    for(i=0;i<months_-1;i++){
        month_percent = year_percent_*getMonthRatio(_dates[i],_dates[i+1]);
        _matrix[i].push_back((double)ceil(100.0*Ostatok*month_percent)/100.0); // выплата по процентам
        tmp = vznos_ - _matrix[i][0];
        Ostatok -= tmp;
        _matrix[i].push_back(tmp);      // выплата по долгу
        _matrix[i].push_back(vznos_);   // взнос
        _matrix[i].push_back(Ostatok);  // остаток после взноса
        percent_sum += _matrix[i][0];   // данные для расчёта последней строки
        debt_sum += _matrix[i][1];
        total_sum += _matrix[i][2];
    }
    // расчёт предпоследней строки
    month_percent = year_percent_*getMonthRatio(_dates[i],_dates[i+1]);
    _matrix[i].push_back((double)ceil(100.0*Ostatok*month_percent)/100.0);
    _matrix[i].push_back(Ostatok);
    tmp = Ostatok + _matrix[i][0];
    _matrix[i].push_back(tmp);
    _matrix[i].push_back(0);

    //расчёт последней строки
    _matrix[i+1].push_back(percent_sum + _matrix[i][0]);
    _matrix[i+1].push_back(debt_sum + _matrix[i][1]);
    _matrix[i+1].push_back(total_sum + _matrix[i][2]);

    if(tmp > vznos_ && Def_payment == 0){   // если последний взнос больше остальных - пересчитать с бОльшим взносом
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
//===========  Увеличивает дату под номером "date_no" на один день  ===============
void Credit::IncDate(const int date_no){
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
//===========  Уменьшает дату под номером "date_no" на один день  ===============
void Credit::DecDate(const int date_no){
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
//==================  Возвращает указатель на рассчитанные даты  =======================
const QDate* Credit::GetDates() const{
    return _dates;
}
//==================  Возвращает указатель на рассчитанные данные  =======================
const std::vector<double>* Credit::GetMatrix() const{
    return _matrix;
}
//==================  Возвращает запрошенную сумму  =======================
double Credit::GetDebt() const{
    return debt_;
}
//==================  Возвращает годовой процент (дробный)  =======================
double Credit::GetPercent() const{
    return year_percent_;
}
//==================  Возвращает количество месяцев  =======================
int Credit::GetMonths() const{
    return months_;
}
//==================  Возвращает сумму ежемесячного платежа  =======================
int Credit::GetMonthPayment() const{
    return vznos_;
}
//======================================================================================
