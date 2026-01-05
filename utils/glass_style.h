/**
 * @file glass_style.h
 * @brief 玻璃拟态风格样式定义
 * 
 * 定义应用程序的玻璃拟态(Glassmorphism)视觉风格，
 * 包括半透明背景、模糊效果、柔和边框等现代UI元素。
 */

#ifndef GLASS_STYLE_H
#define GLASS_STYLE_H

#include <QString>

/**
 * @brief 玻璃拟态风格命名空间
 * 
 * 包含所有玻璃拟态风格的颜色、样式模板和辅助函数
 */
namespace GlassStyle {

    // ========== 主色调 ==========
    const QString PRIMARY_COLOR = "#4facfe";        // 主色 - 柔和蓝色
    const QString PRIMARY_LIGHT = "#00f2fe";        // 浅主色 - 青色
    const QString PRIMARY_DARK = "#3b82f6";         // 深主色 - 蓝色
    
    const QString ACCENT_COLOR = "#a855f7";         // 强调色 - 紫色
    const QString ACCENT_LIGHT = "#c084fc";         // 浅强调色
    
    // ========== 背景色 (渐变基础) - 亮色主题 ==========
    const QString BG_GRADIENT_START = "#4a5568";    // 中灰蓝
    const QString BG_GRADIENT_MID = "#5a67d8";      // 明亮紫蓝
    const QString BG_GRADIENT_END = "#667eea";      // 亮蓝紫
    
    // ========== 玻璃效果色 ==========
    const QString GLASS_BG = "rgba(255, 255, 255, 0.15)";       // 玻璃背景 - 增强可见度
    const QString GLASS_BG_HOVER = "rgba(255, 255, 255, 0.25)";  // 悬停玻璃背景
    const QString GLASS_BG_ACTIVE = "rgba(255, 255, 255, 0.35)"; // 激活玻璃背景
    const QString GLASS_BORDER = "rgba(255, 255, 255, 0.3)";   // 玻璃边框 - 更明显
    const QString GLASS_BORDER_HOVER = "rgba(255, 255, 255, 0.5)"; // 悬停玻璃边框
    
    // ========== 文字颜色 ==========
    const QString TEXT_PRIMARY = "#ffffff";         // 主要文字 - 白色
    const QString TEXT_SECONDARY = "rgba(255, 255, 255, 0.7)"; // 次要文字
    const QString TEXT_MUTED = "rgba(255, 255, 255, 0.5)";     // 淡化文字
    
    // ========== 状态颜色 ==========
    const QString SUCCESS_COLOR = "#10b981";        // 成功 - 绿色
    const QString WARNING_COLOR = "#f59e0b";        // 警告 - 橙色
    const QString ERROR_COLOR = "#ef4444";          // 错误 - 红色
    const QString INFO_COLOR = "#3b82f6";           // 信息 - 蓝色
    
    // ========== 阴影效果 ==========
    const QString SHADOW_LIGHT = "rgba(0, 0, 0, 0.1)";
    const QString SHADOW_MEDIUM = "rgba(0, 0, 0, 0.2)";
    const QString SHADOW_DARK = "rgba(0, 0, 0, 0.3)";
    
    // ========== 主窗口背景渐变样式 - 亮色主题 ==========
    const QString MAIN_BACKGROUND = R"(
        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #4a5568, stop:0.5 #5a67d8, stop:1 #667eea);
        }
        QWidget {
            font-family: "Segoe UI Emoji", "Noto Color Emoji", "Apple Color Emoji", "Microsoft YaHei", "SimHei", "WenQuanYi Micro Hei", "Segoe UI", sans-serif;
        }
    )";
    
    // ========== 顶栏玻璃样式 - 亮色主题 ==========
    const QString TOPBAR_GLASS = R"(
        QFrame#topBar {
            background: rgba(255, 255, 255, 0.2);
            border: none;
            border-bottom: 1px solid rgba(255, 255, 255, 0.25);
        }
        QFrame#topBar QLabel {
            color: #ffffff;
            font-size: 14px;
            padding: 4px 8px;
            background: transparent;
            border: none;
        }
        QFrame#topBar QLabel#labTitle {
            color: #ffffff;
            font-size: 18px;
            font-weight: bold;
            background: transparent;
        }
    )";
    
    // ========== 侧边栏玻璃样式 - 亮色主题 ==========
    const QString SIDEBAR_GLASS = R"(
        QFrame#menuBar {
            background: rgba(255, 255, 255, 0.15);
            border: none;
            border-right: 1px solid rgba(255, 255, 255, 0.2);
        }
    )";
    
    // ========== 导航按钮玻璃样式 - 亮色主题 ==========
    const QString NAV_BUTTON_GLASS = R"(
        QPushButton {
            background: rgba(255, 255, 255, 0.1);
            color: #ffffff;
            border: 1px solid rgba(255, 255, 255, 0.15);
            border-radius: 12px;
            padding: 12px 16px;
            font-size: 15px;
            font-weight: 500;
            text-align: left;
        }
        QPushButton:hover {
            background: rgba(255, 255, 255, 0.25);
            color: #ffffff;
            border: 1px solid rgba(255, 255, 255, 0.35);
        }
        QPushButton:pressed {
            background: rgba(255, 255, 255, 0.35);
        }
        QPushButton:checked {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(79, 172, 254, 0.6), stop:1 rgba(0, 242, 254, 0.4));
            color: #ffffff;
            border: 1px solid rgba(79, 172, 254, 0.7);
            font-weight: bold;
        }
    )";
    
    // ========== 内容区玻璃样式 ==========
    const QString CONTENT_GLASS = R"(
        QStackedWidget#contentStackedWidget {
            background: transparent;
            border: none;
        }
        QStackedWidget#contentStackedWidget > QWidget {
            background: transparent;
        }
    )";
    
    // ========== 页面基础玻璃样式 - 亮色主题 ==========
    const QString PAGE_GLASS = R"(
        QWidget {
            background: transparent;
            color: #ffffff;
        }
        QLabel {
            color: #ffffff;
            background: transparent;
        }
    )";
    
    // ========== 玻璃面板/卡片样式 - 亮色主题 ==========
    const QString GLASS_PANEL = R"(
        QFrame, QGroupBox {
            background: rgba(255, 255, 255, 0.2);
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 16px;
        }
        QGroupBox {
            font-weight: bold;
            font-size: 14px;
            color: #ffffff;
            padding-top: 16px;
            margin-top: 8px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 16px;
            padding: 0 8px;
            background: transparent;
            color: #ffffff;
        }
    )";
    
    // ========== 玻璃按钮样式 - 亮色主题 ==========
    const QString GLASS_BUTTON = R"(
        QPushButton {
            background: rgba(255, 255, 255, 0.2);
            color: #ffffff;
            border: 1px solid rgba(255, 255, 255, 0.35);
            border-radius: 10px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: 500;
        }
        QPushButton:hover {
            background: rgba(255, 255, 255, 0.35);
            border: 1px solid rgba(255, 255, 255, 0.5);
        }
        QPushButton:pressed {
            background: rgba(255, 255, 255, 0.45);
        }
        QPushButton:disabled {
            background: rgba(255, 255, 255, 0.1);
            color: rgba(255, 255, 255, 0.4);
            border: 1px solid rgba(255, 255, 255, 0.15);
        }
    )";
    
    // ========== 主要操作按钮样式 (带渐变) ==========
    const QString GLASS_BUTTON_PRIMARY = R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(79, 172, 254, 0.6), stop:1 rgba(0, 242, 254, 0.4));
            color: #ffffff;
            border: 1px solid rgba(79, 172, 254, 0.5);
            border-radius: 10px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(79, 172, 254, 0.8), stop:1 rgba(0, 242, 254, 0.6));
            border: 1px solid rgba(79, 172, 254, 0.7);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(79, 172, 254, 0.9), stop:1 rgba(0, 242, 254, 0.7));
        }
        QPushButton:disabled {
            background: rgba(79, 172, 254, 0.2);
            color: rgba(255, 255, 255, 0.4);
            border: 1px solid rgba(79, 172, 254, 0.2);
        }
    )";
    
    // ========== 危险操作按钮样式 ==========
    const QString GLASS_BUTTON_DANGER = R"(
        QPushButton {
            background: rgba(239, 68, 68, 0.4);
            color: #ffffff;
            border: 1px solid rgba(239, 68, 68, 0.5);
            border-radius: 10px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: rgba(239, 68, 68, 0.6);
            border: 1px solid rgba(239, 68, 68, 0.7);
        }
        QPushButton:pressed {
            background: rgba(239, 68, 68, 0.75);
        }
    )";
    
    // ========== 玻璃输入框样式 - 亮色主题 ==========
    const QString GLASS_INPUT = R"(
        QLineEdit, QSpinBox, QDoubleSpinBox {
            background: rgba(255, 255, 255, 0.2);
            color: #ffffff;
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 14px;
            selection-background-color: rgba(79, 172, 254, 0.5);
        }
        QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover {
            border: 1px solid rgba(255, 255, 255, 0.45);
        }
        QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border: 1px solid rgba(79, 172, 254, 0.7);
            background: rgba(255, 255, 255, 0.25);
        }
        QSpinBox::up-button, QSpinBox::down-button,
        QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
            background: rgba(255, 255, 255, 0.2);
            border: none;
            width: 20px;
        }
        QSpinBox::up-button:hover, QSpinBox::down-button:hover,
        QDoubleSpinBox::up-button:hover, QDoubleSpinBox::down-button:hover {
            background: rgba(255, 255, 255, 0.35);
        }
    )";
    
    // ========== 玻璃下拉框样式 - 亮色主题 ==========
    const QString GLASS_COMBOBOX = R"(
        QComboBox {
            background: rgba(255, 255, 255, 0.2);
            color: #ffffff;
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 14px;
            min-width: 100px;
        }
        QComboBox:hover {
            border: 1px solid rgba(255, 255, 255, 0.45);
        }
        QComboBox:focus {
            border: 1px solid rgba(79, 172, 254, 0.7);
        }
        QComboBox::drop-down {
            width: 30px;
            border: none;
            background: transparent;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #ffffff;
            margin-right: 10px;
        }
        QComboBox QAbstractItemView {
            background: rgba(90, 103, 216, 0.95);
            color: #ffffff;
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 8px;
            selection-background-color: rgba(79, 172, 254, 0.5);
            selection-color: #ffffff;
            outline: 0;
            padding: 4px;
        }
        QComboBox QAbstractItemView::item {
            padding: 8px 12px;
            border-radius: 4px;
            margin: 2px;
        }
        QComboBox QAbstractItemView::item:hover {
            background: rgba(255, 255, 255, 0.2);
        }
    )";
    
    // ========== 玻璃复选框样式 - 亮色主题 ==========
    const QString GLASS_CHECKBOX = R"(
        QCheckBox {
            color: #ffffff;
            font-size: 14px;
            spacing: 8px;
        }
        QCheckBox::indicator {
            width: 20px;
            height: 20px;
            border-radius: 6px;
            border: 2px solid rgba(255, 255, 255, 0.5);
            background: rgba(255, 255, 255, 0.15);
        }
        QCheckBox::indicator:hover {
            border: 2px solid rgba(255, 255, 255, 0.7);
            background: rgba(255, 255, 255, 0.25);
        }
        QCheckBox::indicator:checked {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgba(79, 172, 254, 0.9), stop:1 rgba(0, 242, 254, 0.7));
            border: 2px solid rgba(79, 172, 254, 0.8);
        }
    )";
    
    // ========== 玻璃列表样式 - 亮色主题 ==========
    const QString GLASS_LIST = R"(
        QListWidget, QListView {
            background: rgba(255, 255, 255, 0.15);
            color: #ffffff;
            border: 1px solid rgba(255, 255, 255, 0.25);
            border-radius: 12px;
            padding: 6px;
            outline: none;
        }
        QListWidget::item, QListView::item {
            background: transparent;
            color: #ffffff;
            border-radius: 8px;
            padding: 10px 14px;
            margin: 2px;
        }
        QListWidget::item:hover, QListView::item:hover {
            background: rgba(255, 255, 255, 0.2);
        }
        QListWidget::item:selected, QListView::item:selected {
            background: rgba(79, 172, 254, 0.5);
            border: 1px solid rgba(79, 172, 254, 0.6);
        }
    )";
    
    // ========== 玻璃标签页样式 - 亮色主题 ==========
    const QString GLASS_TAB = R"(
        QTabWidget::pane {
            background: rgba(255, 255, 255, 0.15);
            border: 1px solid rgba(255, 255, 255, 0.25);
            border-radius: 12px;
            padding: 10px;
        }
        QTabBar::tab {
            background: rgba(255, 255, 255, 0.1);
            color: rgba(255, 255, 255, 0.8);
            border: none;
            padding: 10px 20px;
            margin-right: 4px;
            border-radius: 8px 8px 0 0;
            font-size: 14px;
        }
        QTabBar::tab:hover {
            background: rgba(255, 255, 255, 0.25);
            color: #ffffff;
        }
        QTabBar::tab:selected {
            background: rgba(255, 255, 255, 0.3);
            color: #ffffff;
            font-weight: bold;
        }
    )";
    
    // ========== 玻璃文本编辑器样式 - 亮色主题 ==========
    const QString GLASS_TEXTEDIT = R"(
        QTextEdit, QPlainTextEdit {
            background: rgba(255, 255, 255, 0.15);
            color: #ffffff;
            border: 1px solid rgba(255, 255, 255, 0.25);
            border-radius: 12px;
            padding: 12px;
            font-size: 13px;
            font-family: "Consolas", "Monaco", "Microsoft YaHei", monospace;
            selection-background-color: rgba(79, 172, 254, 0.5);
        }
        QTextEdit:focus, QPlainTextEdit:focus {
            border: 1px solid rgba(79, 172, 254, 0.6);
        }
    )";
    
    // ========== 玻璃滚动条样式 - 亮色主题 ==========
    const QString GLASS_SCROLLBAR = R"(
        QScrollBar:vertical {
            background: transparent;
            width: 10px;
            border-radius: 5px;
            margin: 2px;
        }
        QScrollBar::handle:vertical {
            background: rgba(255, 255, 255, 0.35);
            border-radius: 5px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: rgba(255, 255, 255, 0.5);
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar:horizontal {
            background: transparent;
            height: 10px;
            border-radius: 5px;
            margin: 2px;
        }
        QScrollBar::handle:horizontal {
            background: rgba(255, 255, 255, 0.35);
            border-radius: 5px;
            min-width: 30px;
        }
        QScrollBar::handle:horizontal:hover {
            background: rgba(255, 255, 255, 0.5);
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
    )";
    
    // ========== 玻璃滚动区域样式 ==========
    const QString GLASS_SCROLLAREA = R"(
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollArea > QWidget > QWidget {
            background: transparent;
        }
    )";
    
    // ========== 玻璃对话框样式 - 亮色主题 ==========
    const QString GLASS_DIALOG = R"(
        QDialog {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgba(74, 85, 104, 0.98), stop:0.5 rgba(90, 103, 216, 0.98), stop:1 rgba(102, 126, 234, 0.98));
        }
        QDialog QLabel {
            color: #ffffff;
            background: transparent;
        }
        QDialog QFrame#card {
            background: rgba(255, 255, 255, 0.2);
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 14px;
        }
    )";

    // ========== 完整页面样式组合 ==========
    /**
     * @brief 获取完整的页面样式表
     * @return 组合后的样式表字符串
     */
    inline QString getFullPageStyle() {
        QString style;
        style.reserve(8000);  // Pre-allocate for efficiency
        style.append(PAGE_GLASS);
        style.append(GLASS_PANEL);
        style.append(GLASS_BUTTON);
        style.append(GLASS_INPUT);
        style.append(GLASS_COMBOBOX);
        style.append(GLASS_CHECKBOX);
        style.append(GLASS_LIST);
        style.append(GLASS_TAB);
        style.append(GLASS_TEXTEDIT);
        style.append(GLASS_SCROLLBAR);
        style.append(GLASS_SCROLLAREA);
        return style;
    }

}  // namespace GlassStyle

#endif // GLASS_STYLE_H
