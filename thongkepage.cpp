#include "thongkepage.h"
#include "ui_thongkepage.h"
#include "thongkelogic.h"
#include "inhoadondialog.h"

#include <QDate>
#include <QDateEdit>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTabWidget>

namespace {
Date chuyenQDateSangDate(const QDate& qDate) {
    Date date;
    date.ngay = qDate.day();
    date.thang = qDate.month();
    date.nam = qDate.year();
    return date;
}

QString dinhDangNgay(const Date& date) {
    return QString("%1/%2/%3")
        .arg(date.ngay, 2, 10, QChar('0'))
        .arg(date.thang, 2, 10, QChar('0'))
        .arg(date.nam);
}

QTableWidgetItem* taoItemCanGiua(const QString& text) {
    QTableWidgetItem* item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);
    return item;
}

QTableWidgetItem* taoItemCanPhai(const QString& text) {
    QTableWidgetItem* item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    return item;
}
}

ThongKePage::ThongKePage(TreeVT &rootRef, DS_NHANVIEN &dsRef, QWidget *parent)
    : QWidget(parent), ui(new Ui::ThongKePage), root(rootRef), dsnv(dsRef)
{
    ui->setupUi(this);

    ui->label->setText("Thong ke hoa don va doanh thu");

    QHBoxLayout* boLocLayout = new QHBoxLayout();
    QLabel* tuNgayLabel = new QLabel("Tu ngay:", this);
    QLabel* denNgayLabel = new QLabel("Den ngay:", this);
    QDateEdit* tuNgayEdit = new QDateEdit(QDate::currentDate().addMonths(-1), this);
    QDateEdit* denNgayEdit = new QDateEdit(QDate::currentDate(), this);
    QPushButton* thongKeHoaDonButton = new QPushButton("Thong ke hoa don", this);
    QPushButton* topVatTuButton = new QPushButton("Top 10 vat tu", this);

    tuNgayEdit->setCalendarPopup(true);
    denNgayEdit->setCalendarPopup(true);
    tuNgayEdit->setDisplayFormat("dd/MM/yyyy");
    denNgayEdit->setDisplayFormat("dd/MM/yyyy");

    boLocLayout->addWidget(tuNgayLabel);
    boLocLayout->addWidget(tuNgayEdit);
    boLocLayout->addWidget(denNgayLabel);
    boLocLayout->addWidget(denNgayEdit);
    boLocLayout->addWidget(thongKeHoaDonButton);
    boLocLayout->addWidget(topVatTuButton);
    ui->mainLayout->addLayout(boLocLayout);

    QTabWidget* tabKetQua = new QTabWidget(this);
    QTableWidget* bangHoaDon = new QTableWidget(this);
    QTableWidget* bangTopVatTu = new QTableWidget(this);

    bangHoaDon->setColumnCount(6);
    bangHoaDon->setHorizontalHeaderLabels({"So HD", "Ngay lap", "Loai", "Nhan vien lap", "Tri gia", "Bang chu"});
    bangHoaDon->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    bangHoaDon->setColumnWidth(0, 90);  // So HD
    bangHoaDon->setColumnWidth(1, 110); // Ngay lap
    bangHoaDon->setColumnWidth(2, 80);  // Loai
    bangHoaDon->setColumnWidth(3, 180); // Nhan vien lap
    bangHoaDon->setColumnWidth(4, 130); // Tri gia
    bangHoaDon->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch); // Bang chu
    bangHoaDon->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bangHoaDon->setSelectionBehavior(QAbstractItemView::SelectRows);

    bangTopVatTu->setColumnCount(6);
    bangTopVatTu->setHorizontalHeaderLabels({"STT", "Ma VT", "Ten VT", "So luong xuat", "Doanh thu", "Bang chu"});
    bangTopVatTu->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    bangTopVatTu->setColumnWidth(0, 60);  // STT
    bangTopVatTu->setColumnWidth(1, 90);  // Ma VT
    bangTopVatTu->setColumnWidth(2, 200); // Ten VT
    bangTopVatTu->setColumnWidth(3, 120); // So luong xuat
    bangTopVatTu->setColumnWidth(4, 130); // Doanh thu
    bangTopVatTu->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch); // Bang chu
    bangTopVatTu->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bangTopVatTu->setSelectionBehavior(QAbstractItemView::SelectRows);

    tabKetQua->addTab(bangHoaDon, "Hoa don");
    tabKetQua->addTab(bangTopVatTu, "Top vat tu");
    ui->mainLayout->addWidget(tabKetQua);

    connect(thongKeHoaDonButton, &QPushButton::clicked, this,
            [this, tuNgayEdit, denNgayEdit, bangHoaDon, tabKetQua]() {
        if (tuNgayEdit->date() > denNgayEdit->date()) {
            QMessageBox::warning(this, "Canh bao", "Tu ngay khong duoc lon hon Den ngay!");
            return;
        }

        Date tuNgay = chuyenQDateSangDate(tuNgayEdit->date());
        Date denNgay = chuyenQDateSangDate(denNgayEdit->date());
        DS_THONGKE_HOADON ketQua = thongKeHoaDonTheoThoiGian(dsnv, tuNgay, denNgay);

        bangHoaDon->setRowCount(ketQua.n);
        for (int i = 0; i < ketQua.n; i++) {
            const DongThongKeHoaDon& dong = ketQua.nodes[i];
            bangHoaDon->setItem(i, 0, taoItemCanGiua(dong.soHD));
            bangHoaDon->setItem(i, 1, taoItemCanGiua(dinhDangNgay(dong.ngayLap)));
            bangHoaDon->setItem(i, 2, taoItemCanGiua(dong.loai == 'N' ? "Nhap" : "Xuat"));
            bangHoaDon->setItem(i, 3, new QTableWidgetItem(QString::fromUtf8(dong.hoTenNV.c_str())));
            bangHoaDon->setItem(i, 4, taoItemCanPhai(QString::number(dong.triGia, 'f', 0)));
            
            QString tienChu = QString::fromStdString(docSoThanhChu(static_cast<long long>(dong.triGia)));
            bangHoaDon->setItem(i, 5, new QTableWidgetItem(tienChu));
        }

        huyDSThongKeHoaDon(ketQua);
        tabKetQua->setCurrentWidget(bangHoaDon);
    });

    connect(topVatTuButton, &QPushButton::clicked, this,
            [this, tuNgayEdit, denNgayEdit, bangTopVatTu, tabKetQua]() {
        if (tuNgayEdit->date() > denNgayEdit->date()) {
            QMessageBox::warning(this, "Canh bao", "Tu ngay khong duoc lon hon Den ngay!");
            return;
        }

        Date tuNgay = chuyenQDateSangDate(tuNgayEdit->date());
        Date denNgay = chuyenQDateSangDate(denNgayEdit->date());
        DS_TOP_VATTU_DOANHTHU ketQua = topVatTuDoanhThu(root, dsnv, tuNgay, denNgay, 10);

        bangTopVatTu->setRowCount(ketQua.n);
        for (int i = 0; i < ketQua.n; i++) {
            const DongTopVatTuDoanhThu& dong = ketQua.nodes[i];
            bangTopVatTu->setItem(i, 0, taoItemCanGiua(QString::number(i + 1)));
            bangTopVatTu->setItem(i, 1, taoItemCanGiua(dong.maVT));
            bangTopVatTu->setItem(i, 2, new QTableWidgetItem(QString::fromUtf8(dong.tenVT.c_str())));
            bangTopVatTu->setItem(i, 3, taoItemCanPhai(QString::number(dong.soLuong)));
            bangTopVatTu->setItem(i, 4, taoItemCanPhai(QString::number(dong.doanhThu, 'f', 0)));
            
            QString tienChu = QString::fromStdString(docSoThanhChu(static_cast<long long>(dong.doanhThu)));
            bangTopVatTu->setItem(i, 5, new QTableWidgetItem(tienChu));
        }

        huyDSTopVatTuDoanhThu(ketQua);
        tabKetQua->setCurrentWidget(bangTopVatTu);
    });
}

ThongKePage::~ThongKePage()
{
    delete ui;
}
