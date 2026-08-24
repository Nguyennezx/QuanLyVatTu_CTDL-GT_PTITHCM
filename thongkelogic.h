#ifndef THONGKELOGIC_H
#define THONGKELOGIC_H

#include "cautrucdulieu.h"
#include <string>

// So sanh ngay: <0 neu a truoc b, 0 neu bang, >0 neu a sau b
int soSanhNgay(const Date& a, const Date& b);
bool ngayTrongKhoang(const Date& ngay, const Date& tu, const Date& den);

// Chuan hoa chuoi: xoa khoang trang dau/cuoi, quy cac khoang trang giua ve 1 space
void chuanHoaChuoi(char* s);

// ======================= CHUC NANG (g): THONG KE HOA DON =======================
struct DongThongKeHoaDon {
    char soHD[21];
    Date ngayLap;
    char loai;
    char maNV[11];
    std::string hoTenNV;
    double triGia;
};

const int DELTA_CAPACITY = 5; // Do rong mo rong moi lan day mang (theo yeu cau)

struct DS_THONGKE_HOADON {
    DongThongKeHoaDon* nodes = nullptr;
    int n = 0;
    int capacity = 0; // Suc chua thuc te da cap phat
};

// Them 1 dong hoa don vao danh sach, tu dong mo rong +5 o nho khi day
void themVaoDSHoaDon(DS_THONGKE_HOADON& ds, const DongThongKeHoaDon& dong);

// Thuat toan sap xep cho hoa don (Selection Sort va QuickSort)
void selectionSortHoaDon(DongThongKeHoaDon* a, int n);
void quickSortHoaDon(DongThongKeHoaDon* a, int left, int right);

DS_THONGKE_HOADON thongKeHoaDonTheoThoiGian(const DS_NHANVIEN& dsnv,
                                             const Date& tuNgay,
                                             const Date& denNgay);

void huyDSThongKeHoaDon(DS_THONGKE_HOADON& ds);


// ======================= CHUC NANG (h): TOP 10 VAT TU =======================
struct DongTopVatTuDoanhThu {
    char maVT[11];
    std::string tenVT;
    int soLuong;
    double doanhThu;
};

struct DS_TOP_VATTU_DOANHTHU {
    DongTopVatTuDoanhThu* nodes = nullptr;
    int n = 0;
    int capacity = 0; // Suc chua thuc te da cap phat
};

// Them 1 dong vat tu vao danh sach, tu dong mo rong +5 o nho khi day
void themVaoDSTopVatTu(DS_TOP_VATTU_DOANHTHU& ds, const DongTopVatTuDoanhThu& dong);

// Xoa 1 vat tu theo ma khoi danh sach top vat tu (xoa theo noi dung)
bool xoaVatTuTheoMa(DS_TOP_VATTU_DOANHTHU& ds, const char* maVT);

// Thuat toan sap xep cho top vat tu (Selection Sort va QuickSort)
void selectionSortTopVatTu(DongTopVatTuDoanhThu* a, int n);
void quickSortTopVatTu(DongTopVatTuDoanhThu* a, int left, int right);

DS_TOP_VATTU_DOANHTHU topVatTuDoanhThu(TreeVT root,
                                        const DS_NHANVIEN& dsnv,
                                        const Date& tuNgay,
                                        const Date& denNgay,
                                        int gioiHan = 10);

void huyDSTopVatTuDoanhThu(DS_TOP_VATTU_DOANHTHU& ds);

#endif // THONGKELOGIC_H
