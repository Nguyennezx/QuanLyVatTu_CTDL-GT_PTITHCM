#include "inhoadondialog.h"
#include "hoadonlogic.h"
#include "vattulogic.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QTextEdit>
#include <QHeaderView>
#include <QRegularExpressionValidator>
#include <string>
#include <cctype>
using std::string;

static const string CHUSO_VT[10] = {"không","một","hai","ba","bốn","năm","sáu","bảy","tám","chín"};

static string vietHoaChuCai(const string& s) {
    if (s.empty()) return s;
    string r = s;
    r[0] = toupper((unsigned char)r[0]);
    return r;
}

static string docBaChuSo(int so) {
    string kq = "";
    int tram = so / 100;
    int chuc = (so % 100) / 10;
    int donvi = so % 10;

    if (tram > 0) {
        kq += CHUSO_VT[tram] + " trăm";
        if (chuc == 0 && donvi > 0) kq += " linh";
    }
    if (chuc >= 2) {
        if (!kq.empty()) kq += " ";
        kq += CHUSO_VT[chuc] + " mươi";
        if (donvi == 1) kq += " mốt";
        else if (donvi == 4) kq += " tư";
        else if (donvi == 5) kq += " lăm";
        else if (donvi > 0) kq += " " + CHUSO_VT[donvi];
    } else if (chuc == 1) {
        if (!kq.empty()) kq += " ";
        kq += "mười";
        if (donvi == 5) kq += " lăm";
        else if (donvi > 0) kq += " " + CHUSO_VT[donvi];
    } else if (chuc == 0 && donvi > 0) {
        if (!kq.empty()) kq += " ";
        kq += CHUSO_VT[donvi];
    }
    return kq;
}

static string docNhom(int gia, bool batBuocDu3ChuSo) {
    if (!batBuocDu3ChuSo) return docBaChuSo(gia);
    int tram = gia / 100;
    int chuc = (gia % 100) / 10;
    int donvi = gia % 10;
    string kq = (tram == 0 ? "không" : CHUSO_VT[tram]) + " trăm";
    if (chuc == 0 && donvi > 0) kq += " linh " + CHUSO_VT[donvi];
    else if (chuc == 1) {
        kq += " mười";
        if (donvi == 5) kq += " lăm";
        else if (donvi > 0) kq += " " + CHUSO_VT[donvi];
    } else if (chuc >= 2) {
        kq += " " + CHUSO_VT[chuc] + " mươi";
        if (donvi == 1) kq += " mốt";
        else if (donvi == 4) kq += " tư";
        else if (donvi == 5) kq += " lăm";
        else if (donvi > 0) kq += " " + CHUSO_VT[donvi];
    }
    return kq;
}

static const string DONVI_NHOM[8] = {"", "nghìn", "triệu", "tỷ", "nghìn tỷ", "triệu tỷ", "tỷ tỷ", ""};

static string docSoThanhChu(long long soTien) {
    if (soTien == 0) return "Không đồng";

    int nhom[8];
    int soNhom = 0;
    long long n = soTien;
    while (n > 0 && soNhom < 8) {
        nhom[soNhom] = (int)(n % 1000);
        soNhom++;
        n /= 1000;
    }

    string ketQua = "";
    for (int i = soNhom - 1; i >= 0; i--) {
        int gia = nhom[i];
        bool laNhomDauTien = (i == soNhom - 1);
        if (gia == 0) continue;
        string phan = docNhom(gia, !laNhomDauTien);
        if (!ketQua.empty()) ketQua += " ";
        ketQua += phan;
        if (i < 8 && !DONVI_NHOM[i].empty()) ketQua += " " + DONVI_NHOM[i];
    }
    ketQua += " đồng";
    ketQua = vietHoaChuCai(ketQua);
    return ketQua;
}

InHoaDonDialog::InHoaDonDialog(TreeVT& rootRef, DS_NHANVIEN& dsRef, QWidget* parent)
    : QDialog(parent), root(rootRef), dsnv(dsRef)
{
    setWindowTitle("In hóa đơn");
    resize(700, 700);

    QLabel* danhSachTitle = new QLabel("Chọn 1 hóa đơn trong danh sách, hoặc nhập số HĐ bên dưới:", this);
    danhSachTable = new QTableWidget(this);
    danhSachTable->setColumnCount(4);
    danhSachTable->setHorizontalHeaderLabels({"Số HĐ", "Ngày lập", "Loại", "Người lập"});
    danhSachTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    danhSachTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    danhSachTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    danhSachTable->setMaximumHeight(160);

    soHDEdit = new QLineEdit(this);
    soHDEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[A-Za-z0-9]{0,20}"), this));
    soHDEdit->setPlaceholderText("Hoặc gõ trực tiếp số hóa đơn...");
    timButton = new QPushButton("Tìm", this);

    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: red; font-weight: bold;");
    errorLabel->setVisible(false);
    errorLabel->setWordWrap(true);

    hoaDonView = new QTextEdit(this);
    hoaDonView->setReadOnly(true);
    hoaDonView->setStyleSheet("background: white;");

    QHBoxLayout* timLayout = new QHBoxLayout();
    timLayout->addWidget(new QLabel("Số hóa đơn:", this));
    timLayout->addWidget(soHDEdit);
    timLayout->addWidget(timButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(danhSachTitle);
    mainLayout->addWidget(danhSachTable);
    mainLayout->addLayout(timLayout);
    mainLayout->addWidget(errorLabel);
    mainLayout->addWidget(hoaDonView);

    connect(timButton, &QPushButton::clicked, this, &InHoaDonDialog::onTimClicked);
    connect(soHDEdit, &QLineEdit::returnPressed, this, &InHoaDonDialog::onTimClicked);
    connect(danhSachTable, &QTableWidget::cellClicked, this, &InHoaDonDialog::onChonHDTrongDanhSach);

    napDanhSachHD();
}

void InHoaDonDialog::napDanhSachHD() {
    danhSachTable->setRowCount(0);
    int row = 0;
    for (int i = 0; i < dsnv.n; i++) {
        NHANVIEN* nv = dsnv.nodes[i];
        nodeHD* p = nv->dshd;
        while (p) {
            danhSachTable->insertRow(row);
            QString ngay = QString("%1/%2/%3")
                               .arg(p->hd.NgayLap.ngay, 2, 10, QChar('0'))
                               .arg(p->hd.NgayLap.thang, 2, 10, QChar('0'))
                               .arg(p->hd.NgayLap.nam);
            QString loai = (p->hd.Loai == 'N') ? "Nhập" : "Xuất";
            QString hoTen = QString("%1 %2").arg(nv->HO, nv->TEN);

            danhSachTable->setItem(row, 0, new QTableWidgetItem(p->hd.SoHD));
            danhSachTable->setItem(row, 1, new QTableWidgetItem(ngay));
            danhSachTable->setItem(row, 2, new QTableWidgetItem(loai));
            danhSachTable->setItem(row, 3, new QTableWidgetItem(hoTen));

            p = p->next;
            row++;
        }
    }
}

void InHoaDonDialog::onChonHDTrongDanhSach(int row, int /*column*/) {
    QTableWidgetItem* item = danhSachTable->item(row, 0);
    if (!item) return;
    soHDEdit->setText(item->text());
    onTimClicked();
}

void InHoaDonDialog::xoaBang() {
    hoaDonView->clear();
}

void InHoaDonDialog::onTimClicked() {
    errorLabel->setVisible(false);
    xoaBang();

    QString soHD = soHDEdit->text().trimmed().toUpper();
    if (soHD.isEmpty()) {
        errorLabel->setText("Vui lòng nhập hoặc chọn số hóa đơn!");
        errorLabel->setVisible(true);
        return;
    }

    int idxNV = -1;
    nodeHD* hd = timHoaDonTrongHeThong(dsnv, soHD.toStdString().c_str(), idxNV);
    if (!hd) {
        errorLabel->setText("Không tìm thấy hóa đơn có số: " + soHD);
        errorLabel->setVisible(true);
        return;
    }

    hienThiHoaDon(hd, idxNV);
}

void InHoaDonDialog::hienThiHoaDon(nodeHD* hd, int idxNV) {
    NHANVIEN* nv = dsnv.nodes[idxNV];
    QString hoTen = QString("%1 %2").arg(nv->HO, nv->TEN);
    QString ngay = QString("%1/%2/%3")
                       .arg(hd->hd.NgayLap.ngay, 2, 10, QChar('0'))
                       .arg(hd->hd.NgayLap.thang, 2, 10, QChar('0'))
                       .arg(hd->hd.NgayLap.nam);
    QString loai = (hd->hd.Loai == 'N') ? "PHIẾU NHẬP KHO" : "PHIẾU XUẤT KHO";

    const DS_CTHD& ds = hd->hd.dscthd;

    QString hangHoa;
    for (int i = 0; i < ds.n; i++) {
        const CT_HOADON& ct = ds.nodes[i];
        nodeVT* vtNode = timVT(root, ct.MAVT);
        QString tenVT = vtNode ? QString::fromUtf8(vtNode->vt.TENVT) : "(vật tư đã bị xóa khỏi danh mục)";
        QString dvt = vtNode ? QString::fromUtf8(vtNode->vt.DVT) : "-";
        double thanhTien = tinhTriGiaDong(ct);
        QString mauNen = (i % 2 == 0) ? "#ffffff" : "#f4f6f8";

        hangHoa += QString(
                       "<tr style='background-color:%1;'>"
                       "<td style='padding:7px 8px; border:1px solid #ccc; text-align:center;'>%2</td>"
                       "<td style='padding:7px 8px; border:1px solid #ccc;'>%3</td>"
                       "<td style='padding:7px 8px; border:1px solid #ccc; text-align:center;'>%4</td>"
                       "<td style='padding:7px 8px; border:1px solid #ccc; text-align:right;'>%5</td>"
                       "<td style='padding:7px 8px; border:1px solid #ccc; text-align:right;'>%6</td>"
                       "<td style='padding:7px 8px; border:1px solid #ccc; text-align:right;'>%7</td>"
                       "<td style='padding:7px 8px; border:1px solid #ccc; text-align:right; font-weight:bold;'>%8</td>"
                       "</tr>"
                       ).arg(mauNen).arg(i + 1).arg(tenVT).arg(dvt).arg(ct.SoLuong)
                       .arg(QString::number(ct.DonGia, 'f', 0))
                       .arg(QString::number(ct.VAT, 'f', 1) + "%")
                       .arg(QString::number(thanhTien, 'f', 0));
    }

    double tongTien = tinhTongTriGiaHD(ds);
    QString tienChu = QString::fromStdString(docSoThanhChu(static_cast<long long>(tongTien)));

    QString html = QString(R"(
<div style='font-family:"Times New Roman", serif; background:#ffffff; border:2px solid #333; padding:24px; max-width:680px; margin:auto;'>

  <h1 style='text-align:center; margin:6px 0 4px 0; font-size:20pt;'>%1</h1>
  <p style='text-align:center; margin:0 0 18px 0; font-style:italic; color:#555; font-size:12pt;'>Ngày %3</p>

  <table width='100%%' style='margin-bottom:16px; font-size:11pt;'>
    <tr>
      <td width='30%%'><b>Số phiếu:</b></td><td width='70%%'>%2</td>
    </tr>
    <tr>
      <td width='30%%'><b>Người lập:</b></td><td width='70%%'>%4</td>
    </tr>
  </table>

  <table width='100%%' style='border-collapse:collapse; font-size:10.5pt;'>
    <tr style='background-color:#2c3e50;'>
      <th style='padding:8px; border:1px solid #ccc; color:white;'>STT</th>
      <th style='padding:8px; border:1px solid #ccc; color:white;'>Tên vật tư</th>
      <th style='padding:8px; border:1px solid #ccc; color:white;'>ĐVT</th>
      <th style='padding:8px; border:1px solid #ccc; color:white;'>SL</th>
      <th style='padding:8px; border:1px solid #ccc; color:white;'>Đơn giá</th>
      <th style='padding:8px; border:1px solid #ccc; color:white;'>VAT</th>
      <th style='padding:8px; border:1px solid #ccc; color:white;'>Thành tiền</th>
    </tr>
    %5
  </table>

  <table width='100%%' style='margin-top:10px;'>
    <tr>
      <td style='text-align:right; font-size:13pt; font-weight:bold;'>TỔNG CỘNG:</td>
      <td width='28%%' style='text-align:right; font-size:14pt; font-weight:bold; color:#c0392b;'>%6 VNĐ</td>
    </tr>
  </table>
  <p style='text-align:right; font-style:italic; color:#333; font-size:13pt; margin-top:4px;'>(Bằng chữ: %7)</p>

</div>
)").arg(loai, hd->hd.SoHD, ngay, hoTen, hangHoa,
                            QString::number(tongTien, 'f', 0), tienChu);

    hoaDonView->setHtml(html);
}