#include "mymainwindow.h"

MyMainWindow::MyMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //---------------菜单栏创建---------------
    //获取主窗口的菜单栏
    QMenuBar *bar = menuBar();
    //创建文件菜单
    QMenu *filemenu = bar->addMenu("文件");
    QMenu *editmenu = bar->addMenu("编辑");
    //添加下拉选项
    QAction *NewAction = filemenu->addAction("保存");
    QAction *OpenAction = filemenu->addAction("打开");
    //在下面添加分割线
    filemenu->addSeparator();


    //---------------表格创建---------------
    rows = 10;
    columns = 100;
    // 创建一个10x100的矩阵
    table = new QTableWidget(rows, columns);    //创建一个表格示例(行，列)

    //设置单元格顶部标题列
    for(int i = 1; i < columns; i++)
    {
        QString s = QString("第%1颗").arg(i);
        QTableWidgetItem *item = new QTableWidgetItem(s);
        table->setItem(0,i,item);
    }

    //设置单元格左部标题列
    for(int i = 1; i <rows; i++)
    {
        QString s = QString("时刻%1").arg(i-1);
        QTableWidgetItem *item = new QTableWidgetItem(s);
        table->setItem(i,0,item);
    }
    table->setColumnWidth(0, 40);   //第一列宽度为40

    // 设置每个单元格的初始状态为0
    for(int i=1; i<rows; i++) {
        for(int j=1; j<columns; j++) {
            QTableWidgetItem *item = new QTableWidgetItem("0");
            item->setTextAlignment(Qt::AlignCenter); // 设置文本居中对齐
            table->setColumnWidth(j, 45);
            table->setItem(i, j, item);
        }
    }

    // 监听单元格的点击事件，用于单元格切换数据
    connect(table, &QTableWidget::cellPressed, [=](int row, int col) {
        // 切换状态
        QTableWidgetItem *item = table->item(row, col);
        if(item->row() != 0 && item->column() != 0)
        {
            if(item->text() == "0") {
                item->setText("1");
            } else {
                item->setText("0");
            }
        }
        else{};
    });

    // 监听单元格的变化事件，用于单元格上色
    QBrush RedBrush(QColor(255,0,0));
    QBrush WhiteBrush(QColor(255,255,255));
    connect(table, &QTableWidget::cellChanged, [=](int row, int col) {
        QTableWidgetItem *item = table->item(row, col);
        if(item->text() == "0") {
            item->setBackground(WhiteBrush);
        } else {
            item->setBackground(RedBrush);
        }
    });


    //添加表格控件和菜单栏控件到主窗口中
    setCentralWidget(table);
    setMenuBar(bar);

    resize(1600,710);
    move(0,0);

    //---------------点击保存文件---------------
    connect(NewAction, &QAction::triggered,[=](){
        saveFile();
    });

}



void MyMainWindow::saveFile()
{
    //弹出文件保存对话框，获取用户选择的文件路径和文件名
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    tr("保存表格"),
                                                    "",
                                                    tr("文本文件 (*.txt);;所有文件 (*)"));

    //打开文件，将表格中的数据写入文件
    QFile file(filePath);
    QTextStream out(&file);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {    //以文本方式，只写的打开文件

        for (int i = 1; i < rows; i++) {
            QString line;
            for (int j = 1; j < columns; j++) {
                QTableWidgetItem* item = table->item(i, j);
                if (item && item->text() == "1") {
                    if (line.length() > 0) {
                        line += ",";
                    }
                    line += QString::number(j);
                }
            }
            out << line << ";\n";
        }
    }

    //显示保存结果
    if (file.isOpen()) {
        QMessageBox::information(this, tr("提示"), tr("文件保存成功"));
    } else {
        QMessageBox::warning(this, tr("提示"), tr("文件保存失败"));
    }

    qDebug() << "文件打开标志为：" << file.isOpen();

    //关闭文件
    file.close();
}

MyMainWindow::~MyMainWindow()
{
}
