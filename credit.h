
#ifndef creditH
#define creditH
//---------------------------------------------------------------------------

#include <vector>
#include <memory>
#include <QDate>

#define MIN_MONTHS 3
#define MAX_MONTHS 120

class Credit{
    std::shared_ptr<QDate[]> _dates;    // указатель на даты
    std::shared_ptr<std::vector<double>[]> _matrix; // указатель на результирующую матрицу

    int months_ = 0;        // количество месяцев
    int dates_count = 0;    // количество дат (месяцы + 1)

    double debt_;           // запрошенная сумма
    double year_percent_;   // годовой процент (дробный)
    double vznos_;          // сумма ежемесячного взноса

    void ResetAll();            // стирает весь объект
    void ClearMatrixVectors();  // обнуляет все векторы матрицы
    void BuildAll(const int m); // выделяет память под данные
    double getMonthRatio(const QDate& date1, const QDate& date2);
            // вычисляет отношение числа дней между датами "date1" и "date2" к количеству дней в году

public:
    Credit(){}              // конструктор по-умолчанию
    Credit(const Credit&) = delete;             // удалён конструктор копирования
    Credit operator=(const Credit&) = delete;   // удалён копирующий оператор присваивания
    operator bool() const{        // приведение типа к "bool"
        return bool(_matrix);
    }

    void CalculateDates(const QDate& Date, const int m);
            // строит даты выплат на "m" месяцев, начиная с даты "Date"
    void CalculateCredit(const double Debt, const double Percent, const bool Cents = true, const double Def_payment = 0);
            // ГЛАВНАЯ ФУНКЦИЯ - РАСЧЁТ КРЕДИТА
    void IncDate(const int date_no);    // увеличивает дату под номером "date_no" на один день
    void DecDate(const int date_no);    // уменьшает дату под номером "date_no" на один день

    std::shared_ptr<QDate[]> GetDates() const;  // возвращает указатель на рассчитанные даты
    std::shared_ptr<std::vector<double>[]> GetMatrix() const;   // возвращает указатель на рассчитанные данные
    double GetDebt() const;         // возвращает запрошенную сумму
    double GetPercent() const;      // возвращает годовой процент (дробный)
    int GetMonths() const;          // возвращает количество месяцев
    int GetMonthPayment() const;    // возвращает сумму ежемесячного платежа
};
//---------------------------------------------------------------------------
#endif
