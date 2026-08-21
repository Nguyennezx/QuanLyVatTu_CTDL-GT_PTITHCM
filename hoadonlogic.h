#ifndef HOADONLOGIC_H
#define HOADONLOGIC_H

#include "cautrucdulieu.h"
#include <string>


nodeHD* taoHoaDon(const char* soHD, Date ngay, char loai);
void themHoaDonVaoDS(PTRHD& dshd, nodeHD* hdMoi);
nodeHD* timHoaDon(PTRHD dshd, const char* soHD);

bool themCTHD(DS_CTHD& ds, const char* mavt, int soLuong, float donGia, float vat, std::string& loi);
bool xoaCTHD(DS_CTHD& ds, const char* mavt, std::string& loi);

double tinhTriGiaDong(const CT_HOADON& ct);
double tinhTongTriGiaHD(const DS_CTHD& ds);

void huyDSHoaDon(PTRHD& dshd);

bool isSoHDTonTaiHeThong(const DS_NHANVIEN& dsnv, const char* soHD);
bool kiemTraKhaNangXuatKho(TreeVT root, const DS_CTHD& dsTam, const char* mavt, int soLuongThem, int& tonKho, std::string& loi);
void capNhatTonKhoKhiGhiHD(TreeVT root, const DS_CTHD& ds, char loaiHD);
bool ghiNhanHoaDon(DS_NHANVIEN& dsnv, int idxNV, TreeVT root, const char* soHD, Date ngayLap, char loaiHD, const DS_CTHD& dscthd);


nodeHD* timHoaDonTrongHeThong(const DS_NHANVIEN& dsnv, const char* soHD, int& idxNV);

#endif // HOADONLOGIC_H