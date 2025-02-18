### Unreal Engine Testing Tool

C++ / Assembly Language / Window API / Multithreading / x64DBG / IDA / DirectX / ImGui

---

提供一個強大的解譯器讓開發者獨立於開發環境外做 Unit Testing、Data Tracking，對 Release Version 的功能做測試
，並且使用 DirectX 和 ImGui 在 C++ 上實作簡易 GUI 讓開發者容易使用  
透過此解譯器可取得 Unreal Engine Game Process 中的各類別的詳細資訊，包含 Package、Class、Struct、Enumeration、Function，在 class 中還提供強大的搜尋功能，可找到所有 Inheriting、Base、Outer class，以及當前該 class 的 Instance

---

以下簡單介紹操作介面

1. 主頁面

![本地圖片](https://github.com/JasonHongGG/UEDP/blob/main/Image/1.PNG)

2. 開啟 Process

![本地圖片](https://github.com/JasonHongGG/UEDP/blob/main/Image/2.PNG)

3. 解析物件

- 先選擇需要的選項，如果解析失敗可開啟 AutoConfig 選項
  - 灰 : 關閉選項
  - 橘 : 開啟選項準備執行
  - 綠 : 執行成功
- 點擊 Run 開始解析
- 等待所有解析執行完畢

![本地圖片](https://github.com/JasonHongGG/UEDP/blob/main/Image/3.PNG)

4. 開啟 Console

- 點擊 Side Bar 的螢幕圖示開啟 Console
- 可在右側的兩個紅框內分別查詢 FName 以及 Object 資訊

![本地圖片](https://github.com/JasonHongGG/UEDP/blob/main/Image/4.PNG)

5. Package Viewer

- 整個 Process 相關的物件資訊都在這
- 可利用紅框區塊的搜尋功能快速找尋物件
- EX: 這邊示範找尋 BP_Player_C 的 Class 物件

![本地圖片](https://github.com/JasonHongGG/UEDP/blob/main/Image/5.PNG)

6. Inspector

- 如果輸入 Object Address 則可查看物件繼承關係及 Member 資訊
- 如果輸入 Instance Address 則可以監控物件數據
- 可利用紅框區塊的搜尋功能根據 Object Address 快速找到對應的 Instance Address
- Ex: 利用 BP_Player_C 的 Object Address 找到當前 Process 中的 Instance，在右側 Panel 就可監控相關數據

![本地圖片](https://github.com/JasonHongGG/UEDP/blob/main/Image/6.PNG)

7. Setting

- 因為所有東西皆為物件，所以提供須多客製化的設定
- (注意! 這邊只控制 console 的設定，不影響 main panel(最開始執行解析的 panel))

## ![本地圖片](https://github.com/JasonHongGG/UEDP/blob/main/Image/7.PNG)

### 未來規劃

提供更多 Process 的資訊  
提供視覺化的物件呈現  
提供 API 使開發者可串接
