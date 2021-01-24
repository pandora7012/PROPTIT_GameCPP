Ý tưởng:
Người chơi di chuyển nhân vật vượt qua các chướng ngại vật, thu thập chìa khóa để mở cửa qua màn tiếp theo.
Cách chơi:
  Sử dụng phím A,S,D,W để di chuyển vị trí của nhân vật.
  Có thể sử dụng hộp để có thể che chắn đạn.
  Sau 1 khoảng thời gian nhân vật thả lại 1 cái bóng, sử dụng nút R để nhân vật trở về vi trí của cái bóng đó.
  Khi thu thập hết chìa khóa, di chuyển đến cửa để qua màn chơi tiếp theo.
  Toàn bộ tiến trình của màn chơi sẽ bị reset nếu người chơi trúng đạn
  
Ý nghĩa các Class:
Animation: thực hiện animation cho đối tượng.
AssetManager: quản lý, lưu trữ assets, ngoài ra 1 số hàm tạo object cũng được viết ở đây.
ProjectileComponent: Định nghĩa đạn chướng ngại vật.
ColliderComponent: Định nghĩa về ô collider của Object.
Collision: Check collision của 2 object;
ESC: định nghĩa 1 số phương thức thêm các Component vào object.
KeyboardController: Định nghĩa, xử lý sự kiện bàn phím.
Map: Sinh map từ assets. 
SpriteComponent: Định nghĩa phương thức thêm hình ảnh, animation vào object
TextureManager: Quản lý việc xuất nhập, load file, draw ảnh.
TileComponent: Định nghĩa về 1 số hàm của tileset cho việc sinh map.
TransformComponent: Định nghĩa phương thức về vị trí, di chuyển cho object;
UILabel: Đinh nghĩa phương thức tạo UI cho game.
Vector2D: 
Entity: Định nghĩa chung về 1 object;
Manager : Quản lý , phân loại object.
Component: Định nghĩa chung về thành phần của object.

Hàm core:
Map: Sinh map từ assets:
Hàm map sẽ load 1 file ở bên ngoài là 1 ma trận số nguyên (M x N). Từ file đấy sẽ hiển thị trên game theo assets được được cắt ra theo số pixel được định sẵn.
