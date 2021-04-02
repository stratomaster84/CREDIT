#include <iostream>
#include <iomanip>
#include "credit.h"

using namespace std;

void Print(const Credit& credit){
    // РАСПЕЧАТКА РЕЗУЛЬТАТОВ в консоль
    if(!credit)
        return;

    auto _dates = credit.GetDates();
    auto _matrix = credit.GetMatrix();
    auto _months = credit.GetMonths();

    string date_str; // переменные для построения строки с датой
    int _tmp1,_tmp2; //
    int i;           // счётчик

    cout << setfill(' ');   // заполнить пустоту пробелами
    cout << left;           // выровнить по левому краю

    // первый разделитель
    cout << "|------------|-----------------|--------------|-------------|--------------|" << '\n';

    // заголовочная строка
    cout << "| ";
    cout << setw(11) << "Date" << "| ";
    cout << setw(16) << "Percent payment" << "| ";
    cout << setw(13) << "Debt payment" << "| ";
    cout << setw(12) << "Sum payment" << "| ";
    cout << setw(13) << "Debt balance" << "|" << '\n';
    // второй разделитель
    cout << "|------------|-----------------|--------------|-------------|--------------|" << '\n';

    cout << setprecision(2);    // два знака после запятой
    cout << fixed;              // фиксированное значение при выводе

    // первая дата
    _tmp1 = _dates[0].day();
    _tmp2 = _dates[0].month();
    // формирование строки с датой
    date_str = ((_tmp1<10)?("0"+to_string(_tmp1)):to_string(_tmp1)) + "."
            + ((_tmp2<10)?("0"+to_string(_tmp2)):to_string(_tmp2)) + "."
            + to_string(_dates[0].year());
    cout << "| ";
    cout << setw(11) << date_str << "| ";
    cout << setw(16) << "-" << "| ";
    cout << setw(13) << "-" << "| ";
    cout << setw(12) << "-" << "| ";
    cout << setw(13) << credit.GetDebt() << "|" << '\n';

    // значащие даты
    for(i=0;i<_months;i++){
        _tmp1 = _dates[i+1].day();
        _tmp2 = _dates[i+1].month();
    // формирование строки с датой
        date_str = ((_tmp1<10)?("0"+to_string(_tmp1)):to_string(_tmp1)) + "."
                + ((_tmp2<10)?("0"+to_string(_tmp2)):to_string(_tmp2)) + "."
                + to_string(_dates[i+1].year());
        cout << "| ";
        cout << setw(11) << date_str << "| ";
        cout << setw(16) << _matrix[i][0] << "| ";
        cout << setw(13) << _matrix[i][1] << "| ";
        cout << setw(12) << _matrix[i][2] << "| ";
        cout << setw(13) << _matrix[i][3] << "|" << '\n';
    }
    // третий разделитель
    cout << "|------------|-----------------|--------------|-------------|--------------|" << '\n';

    // последняя строка
    cout << "| ";
    cout << setw(11) << "Total:" << "| ";
    cout << setw(16) << _matrix[i][0] << "| ";
    cout << setw(13) << _matrix[i][1] << "| ";
    cout << setw(12) << _matrix[i][2] << "| ";
    cout << setw(13) << "-" << "|" << '\n';

    // четвёртый разделитель
    cout << "|------------|-----------------|--------------|-------------|--------------|" << '\n';
}

int main(){
    Credit _mine;

    QDate date(2017,02,11);
    int months = 60;
    float debt = 506329;
    float percent = 17;

    _mine.CalculateDates(date,months);
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
    _mine.CalculateCredit(debt,percent);
    Print(_mine);

    return 0;
}







