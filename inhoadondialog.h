#ifndef INHOADONDIALOG_H
#define INHOADONDIALOG_H
#include <QDialog>
#include "cautrucdulieu.h"

class QLineEdit;
class QPushButton;
class QLabel;
class QTableWidget;
class QTextEdit;

class InHoaDonDialog : public QDialog {
    Q_OBJECT
public:
    explicit InHoaDonDialog(TreeVT& rootRef, DS_NHANVIEN& dsRef, QWidget* parent = nullptr);
private slots:
    void onTimClicked();
    void onChonHDTrongDanhSach(int row, int column);
private:
    TreeVT& root;
    DS_NHANVIEN& dsnv;

    QTableWidget* danhSachTable;
    QLineEdit* soHDEdit;
    QPushButton* timButton;
    QLabel* errorLabel;
    QTextEdit* hoaDonView;

    void napDanhSachHD();
    void hienThiHoaDon(nodeHD* hd, int idxNV);
    void xoaBang();
};
#endif // INHOADONDIALOG_H