#include "hoadonpage.h"
#include "ui_hoadonpage.h"
#include "hoadonlogic.h"
#include "vattulogic.h"
#include "nhanvienlogic.h"
#include "fileio.h"
#include "chonvattudialog.h"
#include "inhoadondialog.h"

#include <QDate>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QHeaderView>
#include <QCompleter>
#include <cstring>


HoaDonPage::HoaDonPage(TreeVT &rootRef, DS_NHANVIEN &dsRef, QWidget *parent)
    : QWidget(parent), ui(new Ui::HoaDonPage), root(rootRef), dsnv(dsRef)
{
    ui->setupUi(this);

    // Cấu hình validator cho Số hóa đơn (tối đa 20 ký tự chữ và số)
    ui->soHDEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-z0-9]{0,20}"), this));

    // Cấu hình validator số lượng, đơn giá, VAT
    ui->soLuongEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[1-9][0-9]{0,8}"), this));
    ui->donGiaEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]{1,12}(\\.[0-9]{1,2})?"), this));
    ui->vatEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]{1,2}(\\.[0-9]{1,2})?"), this));

    // Cấu hình ngày lập mặc định là ngày hiện tại và khóa không cho chỉnh sửa
    ui->ngayLapEdit->setDate(QDate::currentDate());
    ui->ngayLapEdit->setEnabled(false);

    // Cấu hình bảng hiển thị chi tiết hóa đơn
    ui->tableCTHD->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableCTHD->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableCTHD->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->errorLabel->setVisible(false);
    cthdTam.n = 0;

    // Kết nối tín hiệu nút bấm
    connect(ui->chonVTButton, &QPushButton::clicked, this, &HoaDonPage::onChonVTClicked);
    connect(ui->themCTButton, &QPushButton::clicked, this, &HoaDonPage::onThemCTClicked);
    connect(ui->xoaCTButton, &QPushButton::clicked, this, &HoaDonPage::onXoaCTClicked);
    connect(ui->ghiButton, &QPushButton::clicked, this, &HoaDonPage::onGhiClicked);
    connect(ui->huyButton, &QPushButton::clicked, this, &HoaDonPage::onHuyClicked);
    connect(ui->inHDButton, &QPushButton::clicked, this, &HoaDonPage::onInHoaDonClicked);

    napDuLieuCombo();
    capNhatBangCTHD();
}

HoaDonPage::~HoaDonPage()
{
    delete ui;
}

void HoaDonPage::napDuLieuCombo() {
    napNVCombo();
}

void HoaDonPage::napNVCombo() {
    ui->nvCombo->clear();
    for (int i = 0; i < dsnv.n; i++) {
        QString itemText = QString("%1 - %2 %3")
                               .arg(dsnv.nodes[i]->MANV)
                               .arg(dsnv.nodes[i]->HO)
                               .arg(dsnv.nodes[i]->TEN);
        ui->nvCombo->addItem(itemText, QString(dsnv.nodes[i]->MANV));
    }
}

void HoaDonPage::onChonVTClicked() {
    ChonVatTuDialog dlg(root, this);
    if (dlg.exec() == QDialog::Accepted) {
        VATTU vt = dlg.getSelectedVatTu();
        selectedMAVT = QString::fromUtf8(vt.MAVT);
        QString textInfo = QString("%1 - %2 (Tồn: %3)")
                               .arg(vt.MAVT)
                               .arg(vt.TENVT)
                               .arg(vt.SoLuongTon);
        ui->vtEdit->setText(textInfo);
        ui->vtEdit->setToolTip(textInfo);
        ui->errorLabel->setVisible(false);
    }
}

void HoaDonPage::onThemCTClicked() {
    ui->errorLabel->setVisible(false);

    QString soHD = ui->soHDEdit->text().trimmed().toUpper();
    if (soHD.isEmpty()) {
        ui->errorLabel->setText("Vui lòng nhập Số hóa đơn trước khi thêm vật tư!");
        ui->errorLabel->setVisible(true);
        return;
    }

    if (ui->nvCombo->currentIndex() < 0) {
        ui->errorLabel->setText("Vui lòng chọn Nhân viên lập hóa đơn!");
        ui->errorLabel->setVisible(true);
        return;
    }

    if (selectedMAVT.isEmpty()) {
        ui->errorLabel->setText("Vui lòng bấm 'Chọn...' để chọn vật tư!");
        ui->errorLabel->setVisible(true);
        return;
    }

    QString mavt = selectedMAVT;
    int soLuong = ui->soLuongEdit->text().toInt();
    float donGia = ui->donGiaEdit->text().toFloat();
    float vat = ui->vatEdit->text().toFloat();

    if (soLuong <= 0) {
        ui->errorLabel->setText("Số lượng phải lớn hơn 0!");
        ui->errorLabel->setVisible(true);
        return;
    }
    if (donGia <= 0) {
        ui->errorLabel->setText("Đơn giá phải lớn hơn 0!");
        ui->errorLabel->setVisible(true);
        return;
    }

    // Kiểm tra tồn kho nếu là Hóa đơn Xuất ('X')
    char loaiHD = (ui->loaiCombo->currentIndex() == 0) ? 'N' : 'X';
    if (loaiHD == 'X') {
        int tonKho = 0;
        std::string loiKiemTra;
        if (!kiemTraKhaNangXuatKho(root, cthdTam, mavt.toStdString().c_str(), soLuong, tonKho, loiKiemTra)) {
            ui->errorLabel->setText(QString::fromStdString(loiKiemTra));
            ui->errorLabel->setVisible(true);
            return;
        }
    }

    std::string loi;
    bool ok = themCTHD(cthdTam, mavt.toStdString().c_str(), soLuong, donGia, vat, loi);
    if (!ok) {
        ui->errorLabel->setText(QString::fromStdString(loi));
        ui->errorLabel->setVisible(true);
        return;
    }

    selectedMAVT.clear();
    ui->vtEdit->clear();
    ui->soLuongEdit->clear();
    ui->donGiaEdit->clear();
    capNhatBangCTHD();
}

void HoaDonPage::onXoaCTClicked() {
    int row = ui->tableCTHD->currentRow();
    if (row < 0 || ui->tableCTHD->selectedItems().isEmpty()) {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn vật tư muốn xóa khỏi hóa đơn đang lập!");
        return;
    }

    QString mavt = ui->tableCTHD->item(row, 0)->text();
    std::string loi;
    xoaCTHD(cthdTam, mavt.toStdString().c_str(), loi);

    capNhatBangCTHD();
}

void HoaDonPage::onGhiClicked() {
    ui->errorLabel->setVisible(false);

    QString soHD = ui->soHDEdit->text().trimmed().toUpper();
    if (soHD.isEmpty()) {
        ui->errorLabel->setText("Vui lòng nhập Số hóa đơn!");
        ui->errorLabel->setVisible(true);
        return;
    }

    if (ui->nvCombo->currentIndex() < 0) {
        ui->errorLabel->setText("Vui lòng chọn Nhân viên lập hóa đơn!");
        ui->errorLabel->setVisible(true);
        return;
    }

    if (cthdTam.n == 0) {
        ui->errorLabel->setText("Hóa đơn phải có ít nhất 1 vật tư!");
        ui->errorLabel->setVisible(true);
        return;
    }

    // Kiểm tra Số HĐ đã tồn tại trong toàn bộ hệ thống chưa
    if (isSoHDTonTaiHeThong(dsnv, soHD.toStdString().c_str())) {
        ui->errorLabel->setText("Số hóa đơn này đã tồn tại trong hệ thống! Vui lòng nhập số HĐ khác.");
        ui->errorLabel->setVisible(true);
        return;
    }

    QString manv = ui->nvCombo->currentData().toString();
    int idxNV = timViTriNV(dsnv, manv.toStdString().c_str());
    if (idxNV == -1) {
        ui->errorLabel->setText("Không tìm thấy nhân viên chọn!");
        ui->errorLabel->setVisible(true);
        return;
    }

    QDate qDate = ui->ngayLapEdit->date();
    Date ngayLap;
    ngayLap.ngay = qDate.day();
    ngayLap.thang = qDate.month();
    ngayLap.nam = qDate.year();

    char loaiHD = (ui->loaiCombo->currentIndex() == 0) ? 'N' : 'X';

    // Xác nhận từ người dùng trước khi ghi
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Xác nhận Ghi Hóa Đơn",
        QString("Bạn có chắc chắn muốn GHI hóa đơn %1 (%2) với %3 vật tư?")
            .arg(soHD)
            .arg(loaiHD == 'N' ? "Nhập" : "Xuất")
            .arg(cthdTam.n),
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) return;

    // Ghi nhận hóa đơn và tự động cập nhật tồn kho trên cây BST Vật tư
    ghiNhanHoaDon(dsnv, idxNV, root, soHD.toStdString().c_str(), ngayLap, loaiHD, cthdTam);

    // Lưu lại dữ liệu ra đĩa
    luuNhanVien(dsnv, FILE_NHANVIEN);
    luuVatTu(root, FILE_VATTU);

    QMessageBox::information(this, "Thành công", 
        QString("Đã ghi hóa đơn %1 thành công! Số lượng tồn kho đã được cập nhật.").arg(soHD));

    // Reset Form
    resetForm();
}

void HoaDonPage::onHuyClicked() {
    resetForm();
}

void HoaDonPage::onInHoaDonClicked() {
    InHoaDonDialog dlg(root, dsnv, this);
    dlg.exec();
}

void HoaDonPage::resetForm() {
    ui->soHDEdit->clear();
    ui->soHDEdit->setEnabled(true);
    ui->nvCombo->setEnabled(true);
    ui->loaiCombo->setEnabled(true);

    selectedMAVT.clear();
    ui->vtEdit->clear();
    ui->vtEdit->setToolTip("");
    ui->soLuongEdit->clear();
    ui->donGiaEdit->clear();
    ui->vatEdit->setText("5");
    ui->ngayLapEdit->setDate(QDate::currentDate());
    ui->loaiCombo->setCurrentIndex(0);
    ui->errorLabel->setVisible(false);

    cthdTam.n = 0;
    capNhatBangCTHD();
}

void HoaDonPage::capNhatBangCTHD() {
    // Nếu hóa đơn đã có ít nhất 1 vật tư đang lập -> Khóa các ô header không cho sửa
    bool dangLap = (cthdTam.n > 0);
    ui->soHDEdit->setEnabled(!dangLap);
    ui->nvCombo->setEnabled(!dangLap);
    ui->loaiCombo->setEnabled(!dangLap);
    ui->xoaCTButton->setEnabled(dangLap);
    ui->ghiButton->setEnabled(dangLap);

    ui->tableCTHD->setRowCount(cthdTam.n);
    double tongTienHD = 0;

    for (int i = 0; i < cthdTam.n; i++) {
        const CT_HOADON& ct = cthdTam.nodes[i];
        nodeVT* node = timVT(root, ct.MAVT);
        QString tenVT = node ? QString::fromUtf8(node->vt.TENVT) : "---";

        double thanhTien = tinhTriGiaDong(ct);
        tongTienHD += thanhTien;

        ui->tableCTHD->setItem(i, 0, new QTableWidgetItem(ct.MAVT));
        ui->tableCTHD->setItem(i, 1, new QTableWidgetItem(tenVT));
        ui->tableCTHD->setItem(i, 2, new QTableWidgetItem(QString::number(ct.SoLuong)));
        ui->tableCTHD->setItem(i, 3, new QTableWidgetItem(QString::number(ct.DonGia, 'f', 0)));
        ui->tableCTHD->setItem(i, 4, new QTableWidgetItem(QString::number(ct.VAT, 'f', 1)));
        ui->tableCTHD->setItem(i, 5, new QTableWidgetItem(QString::number(thanhTien, 'f', 0)));
    }

    QString tienChu = QString::fromStdString(docSoThanhChu(static_cast<long long>(tongTienHD)));
    ui->tongTienLabel->setText(QString("%1 VNĐ\n(Bằng chữ: %2)")
                                   .arg(QString::number(tongTienHD, 'f', 0))
                                   .arg(tienChu));
}