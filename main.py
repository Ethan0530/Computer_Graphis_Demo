from PIL import Image

# 打开图片
img = Image.open('./ava.webp')

# 获取像素数据
pixels = img.load()

# 打印像素坐标和值
for i in range(img.size[0]):    # 图片宽度
    for j in range(img.size[1]):    # 图片高度
        print(f"Pixel value at ({i},{j}): {pixels[i,j]}")
