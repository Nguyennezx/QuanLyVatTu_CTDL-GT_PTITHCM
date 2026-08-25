# Ghi Chú & Yêu Cầu Đồ Án Quản Lý Vật Tư

## 1. Cấp Phát Động vs Cấp Phát Tĩnh
- **So sánh & Ưu điểm cấp phát động**:
  - Cần bao nhiêu cấp phát bấy nhiêu.
  - Khai báo tính toán, nếu sau cần thêm để xử lý có thể mở rộng bộ nhớ.
  - Nếu đầy cấp phát tĩnh: tự mở rộng thêm ô nhớ.
- **Xóa trong danh sách tuyến tính**:
  - Hiểu rõ và nhớ các cách xóa trong danh sách tuyến tính (mảng, mảng con trỏ, danh sách liên kết đơn).

## 2. Quy Tắc & Yêu Cầu Đồ Án
- **Tìm kiếm**:
  - Có thể dùng tham biến nhưng phải đảm bảo không làm thay đổi giá trị (truyền tham chiếu / con trỏ `const`).
  - Tìm kiếm trong danh sách liên kết: Phải kết hợp giữa tìm kiếm thường và tìm kiếm phần tử tăng dần (không nên viết y hệt như tài liệu).
- **Bắt lỗi & Chuẩn hóa dữ liệu**:
  - Phải bắt lỗi và phải bắt hết.
  - Báo có lỗi: Thông báo rõ ràng.
  - Nhập sai ở đâu thì báo lỗi và trỏ con trỏ về chỗ đó để sửa lỗi (`setFocus()`).
  - Tự động sửa lỗi: Giữa các từ cách nhau 1 khoảng trắng, nếu người dùng bỏ khoảng trắng vô thì không nhận, hoặc không cho người dùng nhập vào các ký tự đặc biệt.
  - Không để lỗi xảy ra: Giả sử nếu đi thi, thay vì hiện tất cả danh sách thi của lớp thì chỉ cần hiện danh sách sinh viên chưa thi thôi, còn thi rồi thì ẩn đi.
  - Nên chuẩn hóa dữ liệu với nhau về chung 1 dạng để khi xóa, sửa, thêm dễ xử lý.
- **Xử lý File & Hệ thống**:
  - Thêm Undo và Redo.
  - Vào chương trình phải tự load dữ liệu.
  - Không nhập tới đâu ghi tới đó mà phải xong hết 1 lần rồi mới ghi vào file.

## 3. Thuật Toán Sắp Xếp & Cây Nhị Phân
- Phải biết trình bày các kiểu thuật toán sắp xếp (QuickSort, Selection Sort, Bubble Sort...).
- **Xử lý trên Cây nhị phân**:
  - Không có thuật toán sắp xếp theo cây nhị phân tìm kiếm (Cây không sắp xếp trực tiếp được).
  - Nếu sắp xếp trong cây: Sao chép ra mảng con trỏ cấp phát động rồi mới sắp xếp.
- **Kiểm tra Cây nhị phân**:
  - Phải có thuật toán kiểm tra một cây nhị phân có đúng hay không (tất cả các nút gốc và trung gian phải đủ 2 cây con, tức là bậc phải bằng 2).
