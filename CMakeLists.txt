# =============================================================================
# Orders Plugin - CMakeLists.txt
# =============================================================================
# 这是一个 MPF (Modular Plugin Framework) 插件的 CMake 配置模板。
# 
# 【创建新插件时需要修改的地方】
# 1. project() 中的项目名称 "orders-plugin" -> "your-plugin"
# 2. add_library() 中的目标名称和源文件列表
# 3. qt_add_qml_module() 中的 URI 和 QML 文件列表
# 4. install() 指令中的目标名称
#
# 【依赖说明】
# - MPF::foundation-sdk: 基础SDK，提供插件接口、服务注册等核心功能
# - MPF::mpf-http-client: HTTP客户端库，用于网络请求（可选）
# - MPF::mpf-ui-components: UI组件库，提供统一风格的QML组件（可选）
# =============================================================================

cmake_minimum_required(VERSION 3.21)

# -----------------------------------------------------------------------------
# 【修改点1】项目名称和版本
# 将 "orders-plugin" 改为你的插件名称，如 "inventory-plugin"
# -----------------------------------------------------------------------------
project(orders-plugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)  # Qt MOC 自动处理

# -----------------------------------------------------------------------------
# Qt 依赖
# 根据你的插件功能需求添加或删除 Qt 模块
# - Core: 基础模块（必需）
# - Gui: GUI支持（QML插件必需）
# - Qml: QML引擎（QML插件必需）
# - Quick: Qt Quick（QML插件必需）
# - Network: 网络功能（使用 http-client 库时需要）
# -----------------------------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Network)

# Set Qt policies to avoid warnings
if(COMMAND qt_policy)
    qt_policy(SET QTP0001 NEW)  # QML resource prefix
    qt_policy(SET QTP0004 NEW)  # qmldir for extra directories
endif()

# -----------------------------------------------------------------------------
# MPF SDK 依赖（必需）
# MPF SDK 通过 CMAKE_PREFIX_PATH 指定安装位置
# 在 CMakePresets.json 中配置: "CMAKE_PREFIX_PATH": "C:/Qt/6.8.3/mingw_64;C:/Qt/MPF"
# -----------------------------------------------------------------------------
find_package(MPF REQUIRED)

# -----------------------------------------------------------------------------
# 【修改点2】MPF 可选库依赖
# 根据需要启用或禁用这些库：
# - MPFHttpClient: HTTP客户端，用于 REST API 调用
# - MPFUIComponents: UI组件库，提供 MPFCard、MPFButton 等统一风格组件
# 如果不需要某个库，注释掉对应的 find_package 和 target_link_libraries 行
# -----------------------------------------------------------------------------
find_package(MPFHttpClient REQUIRED)    # HTTP 客户端库
find_package(MPFUIComponents REQUIRED)  # UI 组件库

# -----------------------------------------------------------------------------
# 【修改点3】插件库定义
# 修改目标名称和源文件列表
# 
# 源文件组织建议：
# - src/xxx_plugin.cpp: 插件主类实现
# - src/xxx_service.cpp: 业务服务实现
# - src/xxx_model.cpp: 数据模型实现（如果有）
# - include/xxx_plugin.h: 插件主类头文件
# - include/xxx_service.h: 业务服务头文件
# - include/xxx_model.h: 数据模型头文件
# -----------------------------------------------------------------------------
add_library(orders-plugin SHARED
    # 插件核心
    src/orders_plugin.cpp       # 插件主类 - 实现 IPlugin 接口
    include/orders_plugin.h
    
    # 业务逻辑
    src/orders_service.cpp      # 业务服务 - 提供核心功能
    include/orders_service.h
    
    # 数据模型
    src/order_model.cpp         # QML 列表模型 - 用于 ListView 等
    include/order_model.h
)

target_include_directories(orders-plugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -----------------------------------------------------------------------------
# 【修改点4】链接库
# 根据你的插件需求添加或删除链接库
# -----------------------------------------------------------------------------
target_link_libraries(orders-plugin PRIVATE
    # Qt 模块
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Network              # 使用 http-client 时需要
    
    # MPF SDK（必需）
    MPF::foundation-sdk       # 提供 IPlugin、ServiceRegistry 等接口
    
    # MPF 可选库 - 如不需要可注释掉
    MPF::mpf-http-client      # HTTP 客户端功能
    MPF::mpf-ui-components    # UI 组件（MPFCard、MPFButton 等）
)

# -----------------------------------------------------------------------------
# 【修改点5】QML 模块定义
# 
# URI 命名规范: CompanyName.ModuleName
# - 示例: YourCo.Orders, YourCo.Inventory, MyApp.Users
# 
# QML_FILES: 列出所有 QML 文件
# - 主页面: XxxPage.qml
# - 详情页: XxxDetailPage.qml  
# - 卡片组件: XxxCard.qml
# - 对话框: XxxDialog.qml
#
# OUTPUT_DIRECTORY: QML 模块输出目录，与 URI 对应
# NO_PLUGIN: 表示 QML 模块嵌入到插件 DLL 中，不单独生成 QML 插件
# -----------------------------------------------------------------------------
qt_add_qml_module(orders-plugin
    URI YourCo.Orders         # 【必改】改为你的模块 URI
    VERSION 1.0
    QML_FILES
        qml/OrdersPage.qml           # 主页面
        qml/OrderDetailPage.qml      # 详情页
        qml/OrderCard.qml            # 列表项卡片
        qml/CreateOrderDialog.qml    # 创建对话框
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/YourCo/Orders  # 【必改】与 URI 对应
    NO_PLUGIN  # QML 模块嵌入到主插件 DLL
)

# -----------------------------------------------------------------------------
# 输出目录配置
# 插件 DLL 输出到 build/plugins 目录
# -----------------------------------------------------------------------------
set_target_properties(orders-plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)

# -----------------------------------------------------------------------------
# 安装配置
# 定义 `cmake --install` 时的安装规则
# -----------------------------------------------------------------------------
install(TARGETS orders-plugin
    LIBRARY DESTINATION plugins   # Linux/macOS: .so/.dylib
    RUNTIME DESTINATION plugins   # Windows: .dll
)

install(DIRECTORY ${CMAKE_BINARY_DIR}/qml/YourCo  # 【必改】与 URI 前缀对应
    DESTINATION qml
)
