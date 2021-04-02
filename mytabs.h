#ifndef mytabsH
#define mytabsH
//---------------------------------------------------------------------------

#include <QtWidgets>
#include "credit.h"

//---------------------------------------------------------------------------
class MyTabs : public QTabWidget {
    Q_OBJECT
public:

    Credit credit;
    QTableWidget *table;
    QWidget *tab1;
        QVBoxLayout *left_layout;
            QCalendarWidget *calendar;
            QGroupBox *group_debt;
                QLineEdit *text_debt;
            QGroupBox *group_months;
                QLineEdit *text_months;
            QGroupBox *group_percent;
                QLineEdit *text_percent;
            QPushButton *but_calculate;
            QGroupBox *group_defpayment;
                QLineEdit *text_defpayment;
            QGroupBox *group_prec;
                QRadioButton *but_float;
                QRadioButton *but_int;
        QVBoxLayout *right_layout;
            QGroupBox      *group_chgdates;
                QPushButton *but_dates_up;
                QPushButton *but_dates_down;
            QListWidget *dates_list;
            QPushButton *but_dates;

    MyTabs(QWidget* pwgt = 0);

    void PrintCreditTable(const Credit& credit);   // РАСПЕЧАТКА РЕЗУЛЬТАТОВ в ТАБЛИЦУ
    void PrintDates(const Credit& credit);          // РАСПЕЧАТКА ДАТ
    std::string date_to_string(const QDate& date);  // перевод даты в строку

public slots:
    void slotDatesClicked();    // нажата кнопка расчёта дат
    void slotCalculateClicked(); // нажата кнопка расчёта кредита
    void slotUpClicked();       // нажата кнопка увеличения даты
    void slotDownClicked();     // нажата кнопка уменьшения даты
};
//---------------------------------------------------------------------------
#endif
