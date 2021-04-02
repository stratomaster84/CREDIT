#include "mytabs.h"
#include <string>

// =================КОНСТРУКТОР (СОЗДАНИЕ GUI-интерфейса========================
MyTabs::MyTabs(QWidget* pwgt/*= 0*/) : QTabWidget(pwgt)
{
    QHBoxLayout *Hptr_layout;   // временные указатели
    QVBoxLayout *Vptr_layout;

        // Календарь
            calendar = new QCalendarWidget;
        // Поле с запрошенной суммой
            text_debt = new QLineEdit;
            text_debt->setText("177019");
            Hptr_layout = new QHBoxLayout;
            Hptr_layout->addWidget(text_debt);
            group_debt = new QGroupBox("Запрошенная сумма");
            group_debt->setLayout(Hptr_layout);
        // Поле с запрошенной суммой
            text_months = new QLineEdit;
            text_months->setText("24");
            Hptr_layout = new QHBoxLayout;
            Hptr_layout->addWidget(text_months);
            group_months = new QGroupBox("Количество месяцев");
            group_months->setLayout(Hptr_layout);
        // Поле с запрошенной суммой
            text_percent = new QLineEdit;
            text_percent->setText("12");
            Hptr_layout = new QHBoxLayout;
            Hptr_layout->addWidget(text_percent);
            group_percent = new QGroupBox("Годовая ставка, %");
            group_percent->setLayout(Hptr_layout);
        // Кнопка расчёта кредита
            but_calculate = new QPushButton("Рассчитать кредит");
        // Поле со взносом по-умолчанию
            text_defpayment = new QLineEdit;
            text_defpayment->setText("0");
            Hptr_layout = new QHBoxLayout;
            Hptr_layout->addWidget(text_defpayment);
            group_defpayment = new QGroupBox("Взнос по-умолчанию");
            group_defpayment->setLayout(Hptr_layout);
        // Поле с с точностью
            but_float = new QRadioButton("до центов/копеек");
            but_int = new QRadioButton("до целого");
            Vptr_layout = new QVBoxLayout;
            Vptr_layout->addWidget(but_float);
            Vptr_layout->addWidget(but_int);
            but_float->setChecked(true);
            group_prec = new QGroupBox("Точность");
            group_prec->setLayout(Vptr_layout);
    // Левая часть экрана
        left_layout = new QVBoxLayout;
        left_layout->addWidget(calendar);
        left_layout->addWidget(group_debt);
        left_layout->addWidget(group_months);
        left_layout->addWidget(group_percent);
        left_layout->addWidget(but_calculate);
        left_layout->addWidget(group_defpayment);
        left_layout->addWidget(group_prec);

        // Кнопки изменения дат
            but_dates_up = new QPushButton("+");
            but_dates_down = new QPushButton("-");
            Vptr_layout = new QVBoxLayout;
            Vptr_layout->addWidget(but_dates_up);
            Vptr_layout->addWidget(but_dates_down);
            group_debt = new QGroupBox("Изменить дату");
            group_debt->setLayout(Vptr_layout);
        // Лист с датами
            dates_list = new QListWidget;
        // Кнопка расчёта дат
            but_dates = new QPushButton("Рассчитать\nдаты");
    // Правая часть экрана
        right_layout = new QVBoxLayout;
        right_layout->addWidget(group_debt);
        right_layout->addWidget(dates_list);
        right_layout->addWidget(but_dates);

// Первая вкладка
    Hptr_layout = new QHBoxLayout;
    Hptr_layout->addLayout(left_layout);
    Hptr_layout->addLayout(right_layout);
    tab1 = new QWidget;
    tab1->setLayout(Hptr_layout);
// Вторая вкладка
    table = new QTableWidget;

// ОКНО
    addTab(tab1, "Входные данные");
    addTab(table, "Результат");

//ПРИВЯЗКА КНОПОК
    connect(but_calculate, SIGNAL(clicked()),SLOT(slotCalculateClicked()));
    connect(but_dates,     SIGNAL(clicked()),SLOT(slotDatesClicked()));
    connect(but_dates_up,  SIGNAL(clicked()),SLOT(slotUpClicked()));
    connect(but_dates_down,SIGNAL(clicked()),SLOT(slotDownClicked()));

    //resize(420, 100);
}
// =================  Нажата кнопка расчёта дат  ========================
void MyTabs::slotDatesClicked(){
    QMessageBox msgBox;
    QDate selDate;
    int months;
    try {
        months = std::stoi((char *)(text_months->text().toLatin1().data()));
    }catch(...){
        msgBox.setText("Указано неверное\nколичество месяцев");
        msgBox.exec();
        return;
    }
    selDate = calendar->selectedDate(); // берём начальную дату
    try {
        credit.CalculateDates(selDate, months); // рассчитываем даты
    }catch (const std::string& s) {
        msgBox.setText(s.c_str());
        msgBox.exec();
        return;
    }
    // распечатать даты
    PrintDates(credit);
}
// =================  Нажата кнопка расчёта кредита  ========================
void MyTabs::slotCalculateClicked(){
    double debt;
    double percent;
    double defpayment;
    QMessageBox msgBox;
    try {
        debt = std::stod((char *)(text_debt->text().toLatin1().data()));
    }catch(...){
        msgBox.setText("Указана неверная\nзапрошенная сумма");
        msgBox.exec();
        return;
    }
    try {
        percent = std::stod((char *)(text_percent->text().toLatin1().data()));
    }catch(...){
        msgBox.setText("Указан неверный\nгодовой процент");
        msgBox.exec();
        return;
    }
    try {
        defpayment = std::stod((char *)(text_defpayment->text().toLatin1().data()));
    }catch(...){
        defpayment = 0;  // если "плохой" взнос по-умолчанию, присвоить ему значение 0
    }
    bool cents = false;
    if(but_float->isChecked())
        cents = true;

    if(!credit){            // если ещё не инициализирован
        slotDatesClicked(); // ...то рассчитать даты
        if(!credit)         // если произошла ошибка
            return;         // ...то выйти из функции
    }
    try {
        credit.CalculateCredit(debt,percent,cents,defpayment);  // рассчитать кредит
    }catch (const std::string& s) {
        msgBox.setText(s.c_str());
        msgBox.exec();
        return;
    }
    PrintCreditTable(credit);   // Распечатка результирующей таблицы
    setCurrentIndex(1);         // установка текущей вкладки на "результат"
}
// =================  Нажата кнопка увеличения даты  ========================
void MyTabs::slotUpClicked(){
    if(!credit)
        return;

    int row;
    auto dates = credit.GetDates();
    row = dates_list->currentRow();

    credit.IncDate(row);    // увеличиваем дату
    QListWidgetItem *plwi = dates_list->item(row);  // взять текущую строку в листе
    plwi->setText(date_to_string(dates[row]).c_str());  // присвоить текущей строке новый текст
}
// =================  Нажата кнопка уменьшения даты  ========================
void MyTabs::slotDownClicked(){
    if(!credit)
        return;

    int row;
    auto dates = credit.GetDates();
    row = dates_list->currentRow();

    credit.DecDate(row);    // уменьшаем дату
    QListWidgetItem *plwi = dates_list->item(row);  // взять текущую строку в листе
    plwi->setText(date_to_string(dates[row]).c_str());  // присвоить текущей строке новый текст

}
// =================  РАСПЕЧАТКА РЕЗУЛЬТАТОВ в ТАБЛИЦУ  ========================
void MyTabs::PrintCreditTable(const Credit& credit){
    if(!credit)
        return;
    auto dates = credit.GetDates();
    auto matrix = credit.GetMatrix();
    auto months = credit.GetMonths();
    auto debt = credit.GetDebt();

    int i;           // счётчик
    QTableWidgetItem* ptwi = 0; // указатель на новый элемент таблицы

    table->clear();
    table->setRowCount(months+2);
    table->setColumnCount(5);
    table->verticalHeader()->hide();
    table->setHorizontalHeaderLabels({"Дата","Выплата по\nпроцентам","Выплата\nпо долгу","Суммарная\nвыплата","Долг\nпосле\nвыплаты"});

// РАСПЕЧАТКА ДАТ
    for(i=0;i<=months;i++){
        ptwi = new QTableWidgetItem(date_to_string(dates[i]).c_str());
        table->setItem(i, 0, ptwi);
    }
    ptwi = new QTableWidgetItem("ВСЕГО");
    table->setItem(i, 0, ptwi);

// РАСПЕЧАТКА ПЕРВОЙ СТРОКИ
    ptwi = new QTableWidgetItem("-");
    table->setItem(0, 1, ptwi);
    ptwi = new QTableWidgetItem("-");
    table->setItem(0, 2, ptwi);
    ptwi = new QTableWidgetItem("-");
    table->setItem(0, 3, ptwi);
    ptwi = new QTableWidgetItem(QString("%1").arg(/*ЗНАЧЕНИЕ*/ debt,/*fieldwidth*/ 0,/*fixed*/('f'), /*precision*/ 2));
    table->setItem(0, 4, ptwi);

// РАСПЕЧАТКА ОСНОВНЫХ СТРОК
    for(i=1;i<=months;i++){
        ptwi = new QTableWidgetItem(QString("%1").arg(/*ЗНАЧЕНИЕ*/ matrix[i-1][0],/*fieldwidth*/ 0,/*fixed*/('f'), /*precision*/ 2));
        table->setItem(i, 1, ptwi);
        ptwi = new QTableWidgetItem(QString("%1").arg(matrix[i-1][1],0,'f',2));
        table->setItem(i, 2, ptwi);
        ptwi = new QTableWidgetItem(QString("%1").arg(matrix[i-1][2],0,'f',2));
        table->setItem(i, 3, ptwi);
        ptwi = new QTableWidgetItem(QString("%1").arg(matrix[i-1][3],0,'f',2));
        table->setItem(i, 4, ptwi);
    }
// РАСПЕЧАТКА ПОСЛЕДНЕЙ СТРОКИ
    ptwi = new QTableWidgetItem(QString("%1").arg(/*ЗНАЧЕНИЕ*/ matrix[i-1][0],/*fieldwidth*/ 0,/*fixed*/('f'), /*precision*/ 2));
    table->setItem(i, 1, ptwi);
    ptwi = new QTableWidgetItem(QString("%1").arg(matrix[i-1][1],0,'f',2));
    table->setItem(i, 2, ptwi);
    ptwi = new QTableWidgetItem(QString("%1").arg(matrix[i-1][2],0,'f',2));
    table->setItem(i, 3, ptwi);
    ptwi = new QTableWidgetItem("-");
    table->setItem(i, 4, ptwi);

    table->resizeColumnsToContents();   // подогнать содержимое по ширине...
    table->resizeRowsToContents();      // ...и по высоте
}
// =================  РАСПЕЧАТКА ДАТ  ========================
void MyTabs::PrintDates(const Credit& credit){
    if(!credit)
        return;
    auto dates = credit.GetDates();
    auto months = credit.GetMonths();

    int i;           // счётчик

    dates_list->clear();
// РАСПЕЧАТКА ДАТ
    for(i=0;i<=months;i++){
        new QListWidgetItem(date_to_string(dates[i]).c_str(),dates_list);
    }
    dates_list->setCurrentRow(0);
}
// =================  Перевод даты в строку  ========================
std::string MyTabs::date_to_string(const QDate& date){
    std::string date_str; // переменные для построения строки с датой
    int _tmp1,_tmp2;     // временные переменные
    // формирование строки с датой
    _tmp1 = date.day();
    _tmp2 = date.month();
    date_str = ((_tmp1<10)?("0"+std::to_string(_tmp1)):std::to_string(_tmp1)) + "."
                + ((_tmp2<10)?("0"+std::to_string(_tmp2)):std::to_string(_tmp2)) + "."
                + std::to_string(date.year());
    return date_str;
}

// ======================================================================

