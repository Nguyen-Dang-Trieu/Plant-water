XMEM trong AVR dùng bank 64KB vì con trỏ 16-bit chỉ truy cập được 0x0000 - 0xFFFF tại một thời điểm.

Ngoài 64KB đầu tiên (internal ram + xmem), phần còn lại của SRAM ngoài được chia thành các bank 64KB và
phải chuyển qua thanh ghi RAMPX và RAMPZ.


XMEM_TOTAL_MEMORY = tổng dung lượng SRAM ngoài (theo byte)
Th 1: Tổng dung lượng chia hết cho 64KB

Ví dụ:
- 64KB
- 128KB (2 bank)
- 192KB (3 bank)
- 256KB (4 bank)

=> Địa chỉ cuối bank luôn là 0xFFFF.

TH2: Tổng dung lượng không chia hết cho 64KB

Ví dụ: sử dụng chip 62256 có 32 KB ram

# Các thanh ghi điều khiển extern RAM
- XMCRA và XMCRB
- XMEM tự động phân biệt truy cập nội bộ (internal RAM) hay truy cập bộ nhớ ngoài (external RAM).
    + Nếu là truy cập bộ nhớ ngoài -> XMEM sẽ xuất địa chỉ, dữ liệu và tín hiệu điều khiển (ALE, RD, WR) ra các chân.
    + ALE dùng để phân biệt lúc nào bus AD7:0 là địa chỉ, lúc nào là dữ liệu.
- Kết nói RAM ngoài thường dùng IC 74573

# Vì sao cần 74HC573
- Bus địa chỉ và dữ liệu dùng chung: Trên AVR khi bật XMEM, các chân AD7:0 vừa mang địa chỉ thấp (A7:A0) lúc đầu,
vừa mang theo dữ liệu sau đó. Nghĩa là cùng một nhóm chân thì có 2 trách nhiệm khác nhau tại từng thời điểm.
- Khi ALE lên cao rồi xuống thấp -> AD7:0 truyền địa chỉ sau đó bus này sẽ được dùng để truyền dữ liệu -> Địa chỉ sẽ biến mất.

=> Vậy vai trò của 74HC573 là sẽ giữ lại 8bit địa chỉ thấp khi ALE kích hoạt. Nhờ vậy, địa chỉ vẫn tồn tại ổn định
ở ngõ ra của latch trong suốt quá trình truy cập bộ nhớ, trong bus AD7:0 truyền dữ liệu.

## Thanh ghi XMCRB
- Bit 7 = 1: để tránh floating khi không có dữ liệu truyền.
- Bit 3-0: Chọn số lượng bit địa chỉ cao của XMEM.

VD: 
XMM2	XMM1	XMM0
0        0       0    -> Sử dụng 8bit trên port C từ A8 -> A15
0        0       1    -> Sử dụng 7bit trên port C từ A8 -> A14 => Chân PC7 được sử dụng như IO bình thường.