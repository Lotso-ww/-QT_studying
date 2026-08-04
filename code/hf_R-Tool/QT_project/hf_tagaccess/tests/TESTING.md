# RFID 业务功能整体测试流程

建议先完成无需读写器的自动化测试，再连接真实读写器和 ISO15693 标签进行真机验收。

## 一、自动化测试

这些测试不需要连接读写器，也不需要放置标签。

在 Qt Creator 中逐个打开下面的 `.pro` 文件，选择套件 `Desktop Qt 5.14.2 MSVC2017 32bit`，然后先执行 qmake，再按 `Ctrl+B` 编译、按 `Ctrl+R` 运行。

| 测试工程 | 验证内容 |
| --- | --- |
| `tagpayloadcodec/tst_tagpayloadcodec.pro` | BCD 时间、GBK 姓名编码、病历号校验、payload 编解码、分块补零 |
| `rfidtagservice/tst_rfidtagservice.pro` | 单标签判定、容量与 block 计算、读取、写后校验、首次写入空白标签 |
| `rfidretrypolicy/tst_rfidretrypolicy.pro` | 可重试错误、最多三次、300 ms 与 500 ms 重试延时 |
| `rfidoperationrunner/tst_rfidoperationrunner.pro` | 异步重试与取消 |
| `rfidlogger/tst_rfidlogger.pro` | 日志格式和本地日志脱敏 |
| `rfidlogdispatcher/tst_rfidlogdispatcher.pro` | 界面日志输出、本地日志默认关闭与脱敏 |

每个工程的输出都必须显示 `0 failed`。自动测试阶段不要连接读写器。

## 二、编译主程序

1. 在 Qt Creator 打开 `D:\Gitee.code\qt-learning\code\hf_R-Tool\QT_project\hf_tagaccess\hf_tagaccess.pro`。
2. 确认选择的是 `Desktop Qt 5.14.2 MSVC2017 32bit`，不要选择 Python 或其他套件。
3. 执行 qmake，再按 `Ctrl+B` 编译。
4. 按 `Ctrl+R` 启动。生成的 `debug` 目录旁必须保留 `Drivers` 目录和 RFID SDK 的 DLL 文件。

## 三、真机测试前准备

1. 准备一张可写的 ISO15693 标签。第一次建议使用空白或可擦除的测试标签，因为业务写入从 block 0 开始。
2. 连接读写器，在原有读写器页面选择正确的通信方式和连接参数。
3. 点击 `Open Reader`，确认界面显示连接成功。
4. 勾选实际使用的天线。
5. 切换到原有的单次扫描页面，点击扫描一次，让程序获取附近标签。
6. 确保扫描结果恰好只有一张 ISO15693 标签，再切换到 `Business` 页。扫描到多张标签时会弹窗提示；扫描到零张或多张时，读取和写入按钮都会保持禁用。

## 四、正常读取和写入

1. 将一张标签放在天线感应范围内，点击 `Read Business Data`。
2. 保持标签不移动约 1 秒。
3. 确认界面显示 `Completed`，并检查以下内容：

   - `Stable UID`、`Tag type`、`RSSI`；
   - 原始 HEX；
   - 解析后的皿序号、时间、姓名、病历号；
   - 盘点、连接、读取和完成日志。

4. 输入皿序号、授精时间、女方姓名、病历号。姓名编码后不能超过 8 个 GBK 字节；病历号可填写如 `MR-001`，只允许字母、数字、下划线和连字符。
5. 点击 `Write and Verify`，先核对显示的 HEX，再在确认窗口中点击 `Yes`。
6. 始终保持同一张标签在天线范围内，直到界面显示 `Completed`。
7. 检查最终 HEX 和解析字段与输入完全一致。
8. 再点击一次 `Read Business Data`，确认标签中的数据保持一致。这是最终人工核对步骤。

## 五、异常与边界测试

1. 无标签：先扫描得到 UID，再将标签移开后点击读取。程序最多尝试 3 次，最终应显示盘点超时。
2. 多标签：同时扫描两张标签。程序必须弹窗提示只保留一张标签，`Business` 页不显示可用的单标签状态，读取和写入按钮必须禁用；移开其中一张并重新扫描后，按钮才可恢复。
3. 非法输入：分别测试空病历号、`MR 001`、65 个字符的病历号、GBK 编码超过 8 字节的姓名。写入必须在访问设备前停止，并显示输入错误。
4. 操作中移开标签：开始读取或写入后移开标签。日志应显示第一次失败后等待 300 ms、第二次失败后等待 500 ms，第三次失败后结束。
5. 写后校验：写入期间不要更换标签。写完后自动重新读取，内容一致时成功；不一致时应显示 `VerifyFailed` 并进入重试。
6. 容量不足：若有容量较小的测试标签，写入超过其容量的数据。结果必须是 `CapacityExceeded`，且不得出现部分写入。
7. 本地日志脱敏：勾选 `Enable local file log` 后执行一次写入。日志文件中的姓名应只保留首字符、病历号只保留末 4 位、payload HEX 显示为 `[SENSITIVE_HEX]`。`Include sensitive data in local log` 只用于受控调试；重启程序后它应恢复为关闭状态。

## 六、测试失败时应提供的信息

自动测试失败时，发送 Qt Creator 的完整 Application Output。

真机测试失败时，发送以下信息即可：所选 UID、操作类型、界面中的阶段和错误信息、尝试次数、业务日志文本。不要直接发送未脱敏的本地日志文件。
