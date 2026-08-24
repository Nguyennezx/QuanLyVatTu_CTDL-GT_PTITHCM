#include "thongkelogic.h"
#include "hoadonlogic.h"
#include "vattulogic.h"
#include <cstring>

// So sanh 2 ngay: < 0 neu a truoc b, 0 neu bang nhau, > 0 neu a sau b
int soSanhNgay(const Date& a, const Date& b) {
    if (a.nam != b.nam) return a.nam - b.nam;
    if (a.thang != b.thang) return a.thang - b.thang;
    return a.ngay - b.ngay;
}

// Kiem tra ngay co nam trong khoang [tu, den]
bool ngayTrongKhoang(const Date& ngay, const Date& tu, const Date& den) {
    return soSanhNgay(ngay, tu) >= 0 && soSanhNgay(ngay, den) <= 0;
}

// Chuan hoa chuoi: bo khoang trang dau/cuoi, gom khoang trang thua o giua thanh 1 khoang trang
void chuanHoaChuoi(char* s) {
    if (s == nullptr) return;
    int n = std::strlen(s);
    int i = 0, j = 0;

    // Bo khoang trang dau
    while (i < n && s[i] == ' ') i++;

    bool khoangTrangTruoc = false;
    while (i < n) {
        if (s[i] != ' ') {
            s[j++] = s[i];
            khoangTrangTruoc = false;
        } else if (!khoangTrangTruoc) {
            s[j++] = ' ';
            khoangTrangTruoc = true;
        }
        i++;
    }

    // Bo khoang trang cuoi neu co
    if (j > 0 && s[j - 1] == ' ') j--;
    s[j] = '\0';
}

// ============================================================================
// 1. CAP PHAT DONG & TU DONG MO RONG (+5 O NHO KHI DAY) CHO THONG KE HOA DON
// ============================================================================
void themVaoDSHoaDon(DS_THONGKE_HOADON& ds, const DongThongKeHoaDon& dong) {
    if (ds.n == ds.capacity) {
        int capacityMoi = ds.capacity + DELTA_CAPACITY;
        DongThongKeHoaDon* nodesMoi = new DongThongKeHoaDon[capacityMoi];

        // Sao chep du lieu cu sang vung nho moi
        for (int i = 0; i < ds.n; i++) {
            nodesMoi[i] = ds.nodes[i];
        }

        // Giai phong vung nho cu
        delete[] ds.nodes;
        ds.nodes = nodesMoi;
        ds.capacity = capacityMoi;
    }

    ds.nodes[ds.n++] = dong;
}

// ============================================================================
// 2. SO SANH & SAP XEP HOA DON (SELECTION SORT & QUICKSORT)
// ============================================================================
// Quy tac: Ngay tang dan; neu cung ngay thi SoHD tang dan
static bool hoaDonUuTienTruoc(const DongThongKeHoaDon& a, const DongThongKeHoaDon& b) {
    int cmpNgay = soSanhNgay(a.ngayLap, b.ngayLap);
    if (cmpNgay != 0) return cmpNgay < 0;
    return std::strcmp(a.soHD, b.soHD) < 0;
}

static void hoanViHoaDon(DongThongKeHoaDon& a, DongThongKeHoaDon& b) {
    DongThongKeHoaDon tam = a;
    a = b;
    b = tam;
}

void selectionSortHoaDon(DongThongKeHoaDon* a, int n) {
    for (int i = 0; i < n - 1; i++) {
        int viTriNhoNhat = i;
        for (int j = i + 1; j < n; j++) {
            if (hoaDonUuTienTruoc(a[j], a[viTriNhoNhat])) {
                viTriNhoNhat = j;
            }
        }
        if (viTriNhoNhat != i) {
            hoanViHoaDon(a[i], a[viTriNhoNhat]);
        }
    }
}

void quickSortHoaDon(DongThongKeHoaDon* a, int left, int right) {
    if (left >= right) return;

    DongThongKeHoaDon pivot = a[(left + right) / 2];
    int i = left;
    int j = right;

    while (i <= j) {
        while (hoaDonUuTienTruoc(a[i], pivot)) i++;
        while (hoaDonUuTienTruoc(pivot, a[j])) j--;

        if (i <= j) {
            hoanViHoaDon(a[i], a[j]);
            i++;
            j--;
        }
    }

    if (left < j) quickSortHoaDon(a, left, j);
    if (i < right) quickSortHoaDon(a, i, right);
}

// Chuc nang (g): Thong ke hoa don trong khoang thoi gian
DS_THONGKE_HOADON thongKeHoaDonTheoThoiGian(const DS_NHANVIEN& dsnv,
                                             const Date& tuNgay,
                                             const Date& denNgay) {
    DS_THONGKE_HOADON ketQua;

    for (int i = 0; i < dsnv.n; i++) {
        NHANVIEN* nv = dsnv.nodes[i];
        if (nv == nullptr) continue;

        std::string hoTenNV = std::string(nv->HO) + " " + nv->TEN;

        for (nodeHD* p = nv->dshd; p != nullptr; p = p->next) {
            // Loc hoa don theo khoang thoi gian
            if (!ngayTrongKhoang(p->hd.NgayLap, tuNgay, denNgay)) continue;

            DongThongKeHoaDon dong;
            std::strncpy(dong.soHD, p->hd.SoHD, 20);
            dong.soHD[20] = '\0';
            std::strncpy(dong.maNV, nv->MANV, 10);
            dong.maNV[10] = '\0';
            dong.ngayLap = p->hd.NgayLap;
            dong.loai = p->hd.Loai;
            dong.hoTenNV = hoTenNV;
            dong.triGia = tinhTongTriGiaHD(p->hd.dscthd);

            // Tu dong mo rong bo nho them 5 o khi day
            themVaoDSHoaDon(ketQua, dong);
        }
    }

    // Sap xep hoa don bang QuickSort (O(N log N))
    if (ketQua.n > 1) {
        quickSortHoaDon(ketQua.nodes, 0, ketQua.n - 1);
    }

    return ketQua;
}

void huyDSThongKeHoaDon(DS_THONGKE_HOADON& ds) {
    delete[] ds.nodes;
    ds.nodes = nullptr;
    ds.n = 0;
    ds.capacity = 0;
}


// ============================================================================
// 3. CAP PHAT DONG, MO RONG (+5) & XOA THEO NOI DUNG CHO TOP VAT TU DOANH THU
// ============================================================================
void themVaoDSTopVatTu(DS_TOP_VATTU_DOANHTHU& ds, const DongTopVatTuDoanhThu& dong) {
    if (ds.n == ds.capacity) {
        int capacityMoi = ds.capacity + DELTA_CAPACITY;
        DongTopVatTuDoanhThu* nodesMoi = new DongTopVatTuDoanhThu[capacityMoi];

        for (int i = 0; i < ds.n; i++) {
            nodesMoi[i] = ds.nodes[i];
        }

        delete[] ds.nodes;
        ds.nodes = nodesMoi;
        ds.capacity = capacityMoi;
    }

    ds.nodes[ds.n++] = dong;
}

// Xoa 1 vat tu khoi danh sach theo Ma VT (xoa theo noi dung)
bool xoaVatTuTheoMa(DS_TOP_VATTU_DOANHTHU& ds, const char* maVT) {
    int viTri = -1;
    for (int i = 0; i < ds.n; i++) {
        if (std::strcmp(ds.nodes[i].maVT, maVT) == 0) {
            viTri = i;
            break; // Tim thay thi dung ngay
        }
    }

    if (viTri == -1) return false;

    for (int i = viTri; i < ds.n - 1; i++) {
        ds.nodes[i] = ds.nodes[i + 1];
    }
    ds.n--;
    return true;
}

// Tim kiem tuyen tinh theo Ma VT
static int timDongTopVatTu(const DS_TOP_VATTU_DOANHTHU& ds, const char* maVT) {
    for (int i = 0; i < ds.n; i++) {
        if (std::strcmp(ds.nodes[i].maVT, maVT) == 0) {
            return i; // Tim thay thi dung ngay lap tuc
        }
    }
    return -1;
}

// ============================================================================
// 4. SO SANH & SAP XEP TOP VAT TU (SELECTION SORT & QUICKSORT)
// ============================================================================
// Quy tac: Doanh thu giam dan; neu cung doanh thu thi MaVT tang dan
static bool vatTuUuTienTruoc(const DongTopVatTuDoanhThu& a, const DongTopVatTuDoanhThu& b) {
    if (a.doanhThu != b.doanhThu) return a.doanhThu > b.doanhThu;
    return std::strcmp(a.maVT, b.maVT) < 0;
}

static void hoanViTopVatTu(DongTopVatTuDoanhThu& a, DongTopVatTuDoanhThu& b) {
    DongTopVatTuDoanhThu tam = a;
    a = b;
    b = tam;
}

void selectionSortTopVatTu(DongTopVatTuDoanhThu* a, int n) {
    for (int i = 0; i < n - 1; i++) {
        int viTriLonNhat = i;
        for (int j = i + 1; j < n; j++) {
            if (vatTuUuTienTruoc(a[j], a[viTriLonNhat])) {
                viTriLonNhat = j;
            }
        }
        if (viTriLonNhat != i) {
            hoanViTopVatTu(a[i], a[viTriLonNhat]);
        }
    }
}

void quickSortTopVatTu(DongTopVatTuDoanhThu* a, int left, int right) {
    if (left >= right) return;

    DongTopVatTuDoanhThu pivot = a[(left + right) / 2];
    int i = left;
    int j = right;

    while (i <= j) {
        while (vatTuUuTienTruoc(a[i], pivot)) i++;
        while (vatTuUuTienTruoc(pivot, a[j])) j--;

        if (i <= j) {
            hoanViTopVatTu(a[i], a[j]);
            i++;
            j--;
        }
    }

    if (left < j) quickSortTopVatTu(a, left, j);
    if (i < right) quickSortTopVatTu(a, i, right);
}

// Chuc nang (h): In top vat tu co doanh thu cao nhat trong khoang thoi gian
DS_TOP_VATTU_DOANHTHU topVatTuDoanhThu(TreeVT root,
                                        const DS_NHANVIEN& dsnv,
                                        const Date& tuNgay,
                                        const Date& denNgay,
                                        int gioiHan) {
    DS_TOP_VATTU_DOANHTHU ketQua;

    for (int i = 0; i < dsnv.n; i++) {
        NHANVIEN* nv = dsnv.nodes[i];
        if (nv == nullptr) continue;

        for (nodeHD* p = nv->dshd; p != nullptr; p = p->next) {
            // Chi thong ke tren hoa don Xuat 'X'
            if (p->hd.Loai != 'X') continue;
            if (!ngayTrongKhoang(p->hd.NgayLap, tuNgay, denNgay)) continue;

            for (int j = 0; j < p->hd.dscthd.n; j++) {
                const CT_HOADON& ct = p->hd.dscthd.nodes[j];
                int viTri = timDongTopVatTu(ketQua, ct.MAVT);

                if (viTri == -1) {
                    // Neu chua co trong danh sach thi tao moi va them vao (tu dong mo rong +5 o)
                    DongTopVatTuDoanhThu dongMoi;
                    std::strncpy(dongMoi.maVT, ct.MAVT, 10);
                    dongMoi.maVT[10] = '\0';

                    nodeVT* vt = timVT(root, ct.MAVT);
                    dongMoi.tenVT = vt ? vt->vt.TENVT : "";
                    dongMoi.soLuong = ct.SoLuong;
                    dongMoi.doanhThu = tinhTriGiaDong(ct);

                    themVaoDSTopVatTu(ketQua, dongMoi);
                } else {
                    // Da co thi cong don so luong va doanh thu
                    ketQua.nodes[viTri].soLuong += ct.SoLuong;
                    ketQua.nodes[viTri].doanhThu += tinhTriGiaDong(ct);
                }
            }
        }
    }

    // Sap xep giam dan theo doanh thu bang QuickSort (O(N log N))
    if (ketQua.n > 1) {
        quickSortTopVatTu(ketQua.nodes, 0, ketQua.n - 1);
    }

    // Gioi han top N (vi du top 10)
    if (gioiHan > 0 && ketQua.n > gioiHan) {
        ketQua.n = gioiHan;
    }

    return ketQua;
}

void huyDSTopVatTuDoanhThu(DS_TOP_VATTU_DOANHTHU& ds) {
    delete[] ds.nodes;
    ds.nodes = nullptr;
    ds.n = 0;
    ds.capacity = 0;
}

