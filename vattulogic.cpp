#include "vattulogic.h"
#include <cstring>

nodeVT* taoNodeVT(const char* mavt, const char* tenvt, const char* dvt, int soLuongTon) {
    nodeVT* p = new nodeVT();
    strncpy(p->vt.MAVT, mavt, 10); p->vt.MAVT[10] = '\0';
    strncpy(p->vt.TENVT, tenvt, 50); p->vt.TENVT[50] = '\0';
    strncpy(p->vt.DVT, dvt, 10); p->vt.DVT[10] = '\0';
    p->vt.SoLuongTon = soLuongTon;
    p->left = p->right = nullptr;
    return p;
}

nodeVT* timVT(TreeVT root, const char* mavt) {
    if (!root) return nullptr;
    int cmp = strcmp(mavt, root->vt.MAVT);
    if (cmp == 0) return root;
    return cmp < 0 ? timVT(root->left, mavt) : timVT(root->right, mavt);
}

bool themVT(TreeVT& root, const char* mavt, const char* tenvt, const char* dvt,
            int soLuongTon, std::string& loi) {
    nodeVT** p = &root;
    while (*p) {
        int cmp = strcmp(mavt, (*p)->vt.MAVT);
        if (cmp == 0) { loi = "Ma vat tu da ton tai"; return false; }
        p = (cmp < 0) ? &((*p)->left) :&((*p)->right);
    }

    *p = taoNodeVT(mavt, tenvt, dvt, soLuongTon);
    return true;
}

bool suaVT(TreeVT root, const char* mavt, const char* tenvtMoi, const char* dvtMoi,
           std::string& loi) {
    nodeVT* p = timVT(root, mavt);
    if (!p) { loi = "Khong tim thay vat tu"; return false; }
    if (p->vt.DaXuatHienTrongHD) {
        loi = "Vật tư đã xuất hiện trong hóa đơn! Không được phép sửa";
        return false;
    }
    strncpy(p->vt.TENVT, tenvtMoi, 50); p->vt.TENVT[50] = '\0';
    strncpy(p->vt.DVT, dvtMoi, 10); p->vt.DVT[10] = '\0';
    return true;
}

static void removeMinAndCopy(nodeVT*& r, nodeVT* victim) {
    if (r->left) {
        removeMinAndCopy(r->left, victim);
    } else {
        victim->vt = r->vt;
        nodeVT* t = r;
        r = r->right;
        delete t;
    }
}

bool xoaVT(TreeVT& root, const char* mavt, std::string& loi) {
    if (root == nullptr) {
        loi = "Không tìm thấy vật tư!";
        return false;
    }

    int cmp = strcmp(mavt, root->vt.MAVT);
    if (cmp < 0) {
        return xoaVT(root->left, mavt, loi);
    }
    else if (cmp > 0) {
        return xoaVT(root->right, mavt, loi);
    }
    else {
        if (root->vt.DaXuatHienTrongHD) {
            loi = "Vật tư đã xuất hiện trong hóa đơn! Không được phép xóa";   // ĐỒNG BỘ - có dấu, khớp câu chữ suaVT
            return false;
        }
        // Xoa node la hoac co con trai
        if (!root->right) {
            nodeVT* t = root;
            root = root->left;
            delete t;
        // Co con phai
        } else if (!root->left) {
            nodeVT* t = root;
            root = root->right;
            delete t;
        // co 2 con
        } else {
            removeMinAndCopy(root->right, root);
        }
        return true;
    }
}

int demSoVT(TreeVT root) {
    if (!root) return 0;
    return 1 + demSoVT(root->left) + demSoVT(root->right);
}

static void duyetInorderLayDiaChi(nodeVT* root, nodeVT* mang[], int& idx) {
    if (!root) return;
    duyetInorderLayDiaChi(root->left, mang, idx);
    mang[idx] = root;
    idx++;
    duyetInorderLayDiaChi(root->right, mang, idx);
}

static void bubbleSortTheoTen(nodeVT* mang[], int soLuong) {
    for (int i = 0; i < soLuong - 1; i++) {
        bool coHoanDoi = false;

        for (int j = 0; j < soLuong - 1 - i; j++) {
            if (strcmp(mang[j]->vt.TENVT, mang[j + 1]->vt.TENVT) > 0) {
                nodeVT* tam = mang[j];
                mang[j] = mang[j + 1];
                mang[j + 1] = tam;
                coHoanDoi = true;
            }
        }

        if (!coHoanDoi) break;
    }
}

nodeVT** duyetTheoTen(TreeVT root, int& soLuong) {
    soLuong = demSoVT(root);
    nodeVT** mang = new nodeVT*[soLuong];
    int idx = 0;
    duyetInorderLayDiaChi(root, mang, idx);

    bubbleSortTheoTen(mang, soLuong);
    return mang;
}

void huyCayVT(TreeVT& root) {
    if (!root) return;
    huyCayVT(root->left);
    huyCayVT(root->right);
    delete root;
    root = nullptr;
}