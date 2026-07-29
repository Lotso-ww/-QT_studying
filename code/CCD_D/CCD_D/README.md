# CCD 相机抓拍系统

基于 Qt6 + IDS Peak SDK 开发的 CCD 相机抓拍系统，支持实时预览、单帧抓拍、异步持续保存及历史图像浏览回放。

## 📋 功能特性

### 相机控制
- ✅ 相机连接与断开
- ✅ 曝光时间调节（50μs ~ 30000μs）
- ✅ 增益调节（0dB ~ 36dB）
- ✅ 实时视频流预览（约 30FPS）
- ✅ 单帧抓拍

### 图像保存
- ✅ 单张抓拍自动保存（PNG 格式）
- ✅ 回调异步持续保存（可设置保存上限）
- ✅ 图像数据同时存入 SQLite 数据库
- ✅ 自动创建保存目录

### 历史浏览
- ✅ 会话管理（按程序运行周期分组）
- ✅ 图像列表浏览（上一张/下一张）
- ✅ 图像回放（支持 1x/2x/3x/4x 倍速）
- ✅ 进度条拖动跳转

## 🛠️ 环境要求

| 依赖 | 版本要求 | 说明 |
|------|---------|------|
| Qt | 6.5+ | 推荐 6.5.0，msvc2019_64 版本 |
| IDS Peak SDK | 最新版 | 包含 comfort_sdk 和 generic_sdk |
| Visual Studio | 2019+ | 提供 MSVC 编译器 |
| CMake | 3.19+ | 构建工具 |

## 📦 安装依赖

### 1. Qt 安装
- 下载地址：https://www.qt.io/download
- 安装组件：`Qt 6.5.0` → `MSVC 2019 64-bit`

### 2. IDS Peak SDK 安装
- 下载地址：https://www.ids-imaging.com
- 默认安装路径：`C:\Program Files\IDS\ids_peak\`

## 🔨 编译步骤

### 方法一：使用 CMake（推荐）

```powershell
# 1. 创建构建目录
mkdir build_release
cd build_release

# 2. 配置 CMake（替换为你的 Qt 路径）
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/msvc2019_64"

# 3. 编译
cmake --build . --config Release

# 4. 安装（自动收集依赖）
cmake --install . --config Release --prefix ../release
```

### 方法二：使用 Qt Creator

1. 打开 Qt Creator
2. 选择 `File` → `Open File or Project`
3. 选择项目根目录下的 `CMakeLists.txt`
4. 配置构建套件为 `Desktop Qt 6.5.0 MSVC2019 64bit`
5. 点击 `Build` 按钮

## 🚀 使用方法

### 基本流程

```
连接相机 → 调整参数 → 启动实时流 → 抓拍/保存 → 浏览历史
```

### 详细步骤

#### 1. 连接相机
- 点击「连接相机」按钮
- 程序自动搜索并连接第一个可用相机
- 连接成功后会显示相机型号

#### 2. 调整参数
- **曝光时间**：在输入框中输入数值（单位：μs），按回车或失焦后生效
- **增益**：在输入框中输入数值（单位：dB），按回车或失焦后生效

#### 3. 实时预览
- 点击「实时视频流」按钮
- 右侧显示实时画面（约 30FPS）
- 再次点击停止预览

#### 4. 单帧抓拍
- 预览状态下：截取当前帧保存
- 非预览状态下：同步抓拍一张并保存

#### 5. 持续保存（回调异步）
- 启动实时流后，点击「回调异步」按钮
- 设置保存上限（默认 30 张）
- 持续保存直到达到上限或手动停止

#### 6. 浏览历史图像
- 切换到「图像浏览」标签页
- 在下拉框中选择会话或「全部图片」
- 使用「◀」「▶」按钮切换图像
- 点击「▶」按钮开始自动播放

## 📁 目录结构

```
CCD_D/
├── main.cpp                    # 程序入口
├── mainwindow.h/cpp            # 主窗口（UI 逻辑）
├── mainwindow.ui               # Qt Designer 界面文件
├── camerathread.h/cpp          # 抓帧线程 + 保存工作线程
├── dbmanager.h/cpp             # 数据库管理
├── CMakeLists.txt              # CMake 构建配置
├── README.md                   # 本文件
└── captures/                   # 图像保存目录（运行时自动创建）
    ├── ccd_snap_*.png          # 单帧抓拍图像
    ├── ccd_live_*.png          # 持续保存图像
    └── capture.db              # SQLite 数据库
```

## 🗄️ 数据库结构

```
capture_session (会话表)
├── id          INTEGER PRIMARY KEY
├── start_time  TEXT
├── end_time    TEXT
├── note        TEXT
└── created_at  TEXT

capture_log (抓拍日志)
├── id              INTEGER PRIMARY KEY
├── session_id      INTEGER
├── capture_time    TEXT
├── capture_mode    INTEGER (0=单张, 1=异步)
├── capture_index   INTEGER
├── image_path      TEXT
└── created_at      TEXT

image_data (图像数据)
├── id              INTEGER PRIMARY KEY
├── capture_log_id  INTEGER
├── image_name      TEXT
├── image_path      TEXT
├── image_data      BLOB (PNG)
├── image_size      INTEGER
├── capture_mode    INTEGER
├── capture_time    TEXT
├── session_id      INTEGER
└── created_at      TEXT
```

## ⚠️ 注意事项

1. **相机连接**：使用前请确保相机已正确连接并安装驱动
2. **SDK 版本**：IDS Peak SDK 版本需与代码中引用的头文件路径匹配
3. **数据库位置**：数据库文件位于程序目录下的 `captures/capture.db`
4. **保存目录**：图像文件保存在 `captures/` 目录下
5. **程序退出**：请先停止实时流再关闭程序，避免资源泄漏
6. **权限问题**：程序需要读写权限以创建保存目录和数据库

## 🐛 常见问题

### Q: 连接相机失败？
- 检查相机是否正确连接
- 检查 IDS Peak SDK 是否已安装
- 检查设备管理器中相机是否被正确识别

### Q: 实时画面卡顿？
- 降低曝光时间或帧率
- 确保使用 Release 版本编译
- 关闭其他占用 CPU 的程序

### Q: 保存失败？
- 检查磁盘空间是否充足
- 检查程序是否有写入权限
- 检查保存目录是否被占用

### Q: 数据库打不开？
- 检查 `captures/capture.db` 文件是否存在
- 检查是否有其他程序占用数据库
- 尝试删除 `capture.db-wal` 和 `capture.db-shm` 文件后重试

## 📄 许可证

本项目仅供学习和内部使用。

---

*如有问题，请联系开发者。*