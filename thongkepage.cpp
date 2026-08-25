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

#include <QLocale>
#include <QDialog>
#include <QTextEdit>

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

QString dinhDangTien(double tien) {
    QLocale vn(QLocale::Vietnamese, QLocale::Vietnam);
    return vn.toString(static_cast<long long>(tien));
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

    ui->label->setText("THỐNG KÊ HÓA ĐƠN VÀ DOANH THU");

    QHBoxLayout* boLocLayout = new QHBoxLayout();
    QLabel* tuNgayLabel = new QLabel("Từ ngày:", this);
    QLabel* denNgayLabel = new QLabel("Đến ngày:", this);
    QDateEdit* tuNgayEdit = new QDateEdit(QDate::currentDate().addMonths(-1), this);
    QDateEdit* denNgayEdit = new QDateEdit(QDate::currentDate(), this);
    QPushButton* thongKeHoaDonButton = new QPushButton("Thống kê hóa đơn", this);
    QPushButton* topVatTuButton = new QPushButton("Top 10 vật tư", this);
    QPushButton* inBaoCaoButton = new QPushButton("In / Xem mẫu", this);

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
    boLocLayout->addWidget(inBaoCaoButton);
    ui->mainLayout->addLayout(boLocLayout);

    QTabWidget* tabKetQua = new QTabWidget(this);
    QTableWidget* bangHoaDon = new QTableWidget(this);
    QTableWidget* bangTopVatTu = new QTableWidget(this);

    // Bảng hóa đơn: Đúng chuẩn 5 cột theo đề bài (Số HĐ, Ngày lập, Loại HĐ, Họ tên NV lập, Trị giá hóa đơn)
    bangHoaDon->setColumnCount(5);
    bangHoaDon->setHorizontalHeaderLabels({"Số HĐ", "Ngày lập", "Loại HĐ", "Họ tên NV lập", "Trị giá hóa đơn"});
    bangHoaDon->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    bangHoaDon->setColumnWidth(0, 110); // Số HĐ
    bangHoaDon->setColumnWidth(1, 120); // Ngày lập
    bangHoaDon->setColumnWidth(2, 100); // Loại HĐ
    bangHoaDon->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch); // Họ tên NV lập
    bangHoaDon->setColumnWidth(4, 180); // Trị giá hóa đơn
    bangHoaDon->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bangHoaDon->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Bảng Top 10 vật tư: 5 cột chuẩn
    bangTopVatTu->setColumnCount(5);
    bangTopVatTu->setHorizontalHeaderLabels({"STT", "Mã VT", "Tên VT", "Số lượng xuất", "Doanh thu"});
    bangTopVatTu->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    bangTopVatTu->setColumnWidth(0, 70);  // STT
    bangTopVatTu->setColumnWidth(1, 110); // Mã VT
    bangTopVatTu->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // Tên VT
    bangTopVatTu->setColumnWidth(3, 140); // Số lượng xuất
    bangTopVatTu->setColumnWidth(4, 180); // Doanh thu
    bangTopVatTu->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bangTopVatTu->setSelectionBehavior(QAbstractItemView::SelectRows);

    tabKetQua->addTab(bangHoaDon, "Hóa đơn trong khoảng thời gian");
    tabKetQua->addTab(bangTopVatTu, "Top 10 vật tư doanh thu cao nhất");
    ui->mainLayout->addWidget(tabKetQua);

    connect(thongKeHoaDonButton, &QPushButton::clicked, this,
            [this, tuNgayEdit, denNgayEdit, bangHoaDon, tabKetQua]() {
        if (tuNgayEdit->date() > denNgayEdit->date()) {
            QMessageBox::warning(this, "Cảnh báo", "Từ ngày không được lớn hơn Đến ngày!");
            return;
        }

        Date tuNgay = chuyenQDateSangDate(tuNgayEdit->date());
        Date denNgay = chuyenQDateSangDate(denNgayEdit->date());

        // Tiêu đề kết xuất đúng mẫu yêu cầu
        QString tieuDe = QString("BẢNG LIỆT KÊ CÁC HÓA ĐƠN TRONG KHOẢNG THỜI GIAN\nTừ ngày : %1   Đến ngày : %2")
                             .arg(dinhDangNgay(tuNgay))
                             .arg(dinhDangNgay(denNgay));
        ui->label->setText(tieuDe);

        DS_THONGKE_HOADON ketQua = thongKeHoaDonTheoThoiGian(dsnv, tuNgay, denNgay);

        bangHoaDon->setRowCount(ketQua.n);
        for (int i = 0; i < ketQua.n; i++) {
            const DongThongKeHoaDon& dong = ketQua.nodes[i];
            bangHoaDon->setItem(i, 0, taoItemCanGiua(dong.soHD));
            bangHoaDon->setItem(i, 1, taoItemCanGiua(dinhDangNgay(dong.ngayLap)));
            bangHoaDon->setItem(i, 2, taoItemCanGiua(dong.loai == 'N' ? "Nhập" : "Xuất"));
            bangHoaDon->setItem(i, 3, new QTableWidgetItem(QString::fromUtf8(dong.hoTenNV.c_str())));
            bangHoaDon->setItem(i, 4, taoItemCanPhai(dinhDangTien(dong.triGia)));
        }

        huyDSThongKeHoaDon(ketQua);
        tabKetQua->setCurrentWidget(bangHoaDon);
    });

    connect(topVatTuButton, &QPushButton::clicked, this,
            [this, tuNgayEdit, denNgayEdit, bangTopVatTu, tabKetQua]() {
        if (tuNgayEdit->date() > denNgayEdit->date()) {
            QMessageBox::warning(this, "Cảnh báo", "Từ ngày không được lớn hơn Đến ngày!");
            return;
        }

        Date tuNgay = chuyenQDateSangDate(tuNgayEdit->date());
        Date denNgay = chuyenQDateSangDate(denNgayEdit->date());

        // Tiêu đề kết xuất đúng mẫu yêu cầu
        QString tieuDe = QString("BẢNG LIỆT KÊ 10 VẬT TƯ CÓ DOANH THU CAO NHẤT\nTừ ngày : %1   Đến ngày : %2")
                             .arg(dinhDangNgay(tuNgay))
                             .arg(dinhDangNgay(denNgay));
        ui->label->setText(tieuDe);

        DS_TOP_VATTU_DOANHTHU ketQua = topVatTuDoanhThu(root, dsnv, tuNgay, denNgay, 10);

        bangTopVatTu->setRowCount(ketQua.n);
        for (int i = 0; i < ketQua.n; i++) {
            const DongTopVatTuDoanhThu& dong = ketQua.nodes[i];
            bangTopVatTu->setItem(i, 0, taoItemCanGiua(QString::number(i + 1)));
            bangTopVatTu->setItem(i, 1, taoItemCanGiua(dong.maVT));
            bangTopVatTu->setItem(i, 2, new QTableWidgetItem(QString::fromUtf8(dong.tenVT.c_str())));
            bangTopVatTu->setItem(i, 3, taoItemCanPhai(QString::number(dong.soLuong)));
            bangTopVatTu->setItem(i, 4, taoItemCanPhai(dinhDangTien(dong.doanhThu)));
        }

        huyDSTopVatTuDoanhThu(ketQua);
        tabKetQua->setCurrentWidget(bangTopVatTu);
    });

    // Sự kiện mở cửa sổ mẫu in văn bản chuẩn chỉnh
    connect(inBaoCaoButton, &QPushButton::clicked, this,
            [this, tuNgayEdit, denNgayEdit, tabKetQua]() {
        if (tuNgayEdit->date() > denNgayEdit->date()) {
            QMessageBox::warning(this, "Cảnh báo", "Từ ngày không được lớn hơn Đến ngày!");
            return;
        }

        Date tuNgay = chuyenQDateSangDate(tuNgayEdit->date());
        Date denNgay = chuyenQDateSangDate(denNgayEdit->date());

        QDialog dlg(this);
        dlg.setWindowTitle("Xem mẫu in kết xuất");
        dlg.resize(800, 600);

        QVBoxLayout* dlgLayout = new QVBoxLayout(&dlg);
        QTextEdit* view = new QTextEdit(&dlg);
        view->setReadOnly(true);
        dlgLayout->addWidget(view);

        QString html;

        if (tabKetQua->currentIndex() == 0) {
            // Mẫu in Thống kê Hóa đơn
            DS_THONGKE_HOADON ketQua = thongKeHoaDonTheoThoiGian(dsnv, tuNgay, denNgay);

            QString hangHoaHtml;
            for (int i = 0; i < ketQua.n; i++) {
                const DongThongKeHoaDon& dong = ketQua.nodes[i];
                QString loaiStr = (dong.loai == 'N') ? "Nhập" : "Xuất";
                QString mauNen = (i % 2 == 0) ? "#ffffff" : "#fbfbfb";
                hangHoaHtml += QString(
                    "<tr style='background-color:%1;'>"
                    "<td align='center' style='padding:7px 8px; border:1px solid #333;'>%2</td>"
                    "<td align='center' style='padding:7px 8px; border:1px solid #333;'>%3</td>"
                    "<td align='center' style='padding:7px 8px; border:1px solid #333;'>%4</td>"
                    "<td align='left' style='padding:7px 8px; border:1px solid #333;'>&nbsp;%5</td>"
                    "<td align='right' style='padding:7px 8px; border:1px solid #333;'>%6&nbsp;</td>"
                    "</tr>"
                ).arg(mauNen, dong.soHD, dinhDangNgay(dong.ngayLap), loaiStr,
                      QString::fromUtf8(dong.hoTenNV.c_str()), dinhDangTien(dong.triGia));
            }

            html = QString(R"(
<div style='font-family:"Times New Roman", serif; padding:20px; background-color:#ffffff;'>
  <h2 align='center' style='margin-bottom:6px; font-size:18pt; text-transform:uppercase;'>BẢNG LIỆT KÊ CÁC HÓA ĐƠN TRONG KHOẢNG THỜI GIAN</h2>
  <p align='center' style='margin-top:0px; margin-bottom:22px; font-size:13pt;'>
    <b>Từ ngày :</b> %1 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <b>Đến ngày :</b> %2
  </p>
  <center>
  <table align='center' width='98%%' border='1' cellspacing='0' cellpadding='7' style='border-collapse:collapse; font-size:12pt; border:1px solid #333;'>
    <tr style='background-color:#e8e8e8; font-weight:bold;'>
      <th align='center' style='padding:8px; width:15%%;'>Số HĐ</th>
      <th align='center' style='padding:8px; width:15%%;'>Ngày lập</th>
      <th align='center' style='padding:8px; width:12%%;'>Loại HĐ</th>
      <th align='center' style='padding:8px; width:33%%;'>Họ tên NV lập</th>
      <th align='center' style='padding:8px; width:25%%;'>Trị giá hóa đơn</th>
    </tr>
    %3
  </table>
  </center>
</div>
            )").arg(dinhDangNgay(tuNgay), dinhDangNgay(denNgay), hangHoaHtml);

            huyDSThongKeHoaDon(ketQua);
        } else {
            // Mẫu in Top 10 vật tư
            DS_TOP_VATTU_DOANHTHU ketQua = topVatTuDoanhThu(root, dsnv, tuNgay, denNgay, 10);

            QString hangHoaHtml;
            for (int i = 0; i < ketQua.n; i++) {
                const DongTopVatTuDoanhThu& dong = ketQua.nodes[i];
                QString mauNen = (i % 2 == 0) ? "#ffffff" : "#fbfbfb";
                hangHoaHtml += QString(
                    "<tr style='background-color:%1;'>"
                    "<td align='center' style='padding:7px 8px; border:1px solid #333;'>%2</td>"
                    "<td align='center' style='padding:7px 8px; border:1px solid #333;'>%3</td>"
                    "<td align='left' style='padding:7px 8px; border:1px solid #333;'>&nbsp;%4</td>"
                    "<td align='right' style='padding:7px 8px; border:1px solid #333;'>%5&nbsp;</td>"
                    "<td align='right' style='padding:7px 8px; border:1px solid #333;'>%6&nbsp;</td>"
                    "</tr>"
                ).arg(mauNen).arg(i + 1).arg(dong.maVT).arg(QString::fromUtf8(dong.tenVT.c_str()))
                 .arg(dong.soLuong).arg(dinhDangTien(dong.doanhThu));
            }

            html = QString(R"(
<div style='font-family:"Times New Roman", serif; padding:20px; background-color:#ffffff;'>
  <h2 align='center' style='margin-bottom:6px; font-size:18pt; text-transform:uppercase;'>BẢNG LIỆT KÊ 10 VẬT TƯ CÓ DOANH THU CAO NHẤT</h2>
  <p align='center' style='margin-top:0px; margin-bottom:22px; font-size:13pt;'>
    <b>Từ ngày :</b> %1 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <b>Đến ngày :</b> %2
  </p>
  <center>
  <table align='center' width='98%%' border='1' cellspacing='0' cellpadding='7' style='border-collapse:collapse; font-size:12pt; border:1px solid #333;'>
    <tr style='background-color:#e8e8e8; font-weight:bold;'>
      <th align='center' style='padding:8px; width:10%%;'>STT</th>
      <th align='center' style='padding:8px; width:15%%;'>Mã VT</th>
      <th align='center' style='padding:8px; width:35%%;'>Tên VT</th>
      <th align='center' style='padding:8px; width:18%%;'>Số lượng xuất</th>
      <th align='center' style='padding:8px; width:22%%;'>Doanh thu</th>
    </tr>
    %3
  </table>
  </center>
</div>
            )").arg(dinhDangNgay(tuNgay), dinhDangNgay(denNgay), hangHoaHtml);

            huyDSTopVatTuDoanhThu(ketQua);
        }

        view->setStyleSheet("background-color: white;");
        view->setHtml(html);
        dlg.exec();
    });
}

ThongKePage::~ThongKePage()
{
    delete ui;
}
