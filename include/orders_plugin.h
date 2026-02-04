/**
 * =============================================================================
 * Orders Plugin - 插件主类头文件
 * =============================================================================
 * 
 * 这是 MPF 插件的主类定义模板。每个插件都需要一个实现 IPlugin 接口的主类。
 * 
 * 【创建新插件时需要修改的地方】
 * 1. 命名空间: orders -> your_namespace
 * 2. 类名: OrdersPlugin -> YourPlugin
 * 3. Q_PLUGIN_METADATA 中的 FILE 路径: "../orders_plugin.json" -> "../your_plugin.json"
 * 4. qmlModuleUri() 返回值: "YourCo.Orders" -> "YourCo.YourModule"
 * 5. 服务类: OrdersService -> YourService
 * 
 * 【IPlugin 接口说明】
 * - initialize(): 插件初始化，创建服务、注册 QML 类型
 * - start(): 插件启动，注册路由、菜单、加载数据
 * - stop(): 插件停止，清理资源
 * - metadata(): 返回插件元数据（应与 JSON 文件一致）
 * - qmlModuleUri(): 返回 QML 模块 URI
 * - entryQml(): 如果插件提供应用入口 QML，在此返回路径
 * =============================================================================
 */

#pragma once

#include <mpf/interfaces/iplugin.h>  // MPF 插件接口
#include <QObject>

// 【修改点1】命名空间 - 改为你的插件命名空间
namespace orders {

// 前向声明 - 【修改点2】改为你的服务类名
class OrdersService;

/**
 * @brief 订单管理插件主类
 * 
 * 【功能演示】
 * 这个示例插件展示了以下功能：
 * - 服务创建和管理 (OrdersService)
 * - QML 类型注册 (qmlRegisterSingletonInstance, qmlRegisterType)
 * - 路由注册 (INavigation::registerRoute)
 * - 菜单项注册 (IMenu::registerItem)
 * - 菜单徽章更新 (IMenu::setBadge)
 * - 使用 MPF 日志系统 (MPF_LOG_xxx)
 * 
 * 【创建新插件清单】
 * 1. 复制此文件，重命名类和命名空间
 * 2. 创建对应的 .cpp 实现文件
 * 3. 创建 xxx_plugin.json 元数据文件
 * 4. 在 CMakeLists.txt 中添加源文件
 */
// 【修改点3】类名 - 改为你的插件类名
class OrdersPlugin : public QObject, public mpf::IPlugin
{
    Q_OBJECT
    
    // =========================================================================
    // 【修改点4】插件元数据宏
    // - IID: 保持不变，这是 MPF 插件的统一标识符
    // - FILE: 改为你的插件 JSON 文件路径（相对于此头文件）
    // =========================================================================
    Q_PLUGIN_METADATA(IID MPF_IPlugin_iid FILE "../orders_plugin.json")
    Q_INTERFACES(mpf::IPlugin)

public:
    explicit OrdersPlugin(QObject* parent = nullptr);
    ~OrdersPlugin() override;

    // =========================================================================
    // IPlugin 接口实现
    // =========================================================================
    
    /**
     * @brief 初始化插件
     * 
     * 在此方法中：
     * - 保存 ServiceRegistry 引用
     * - 创建业务服务实例
     * - 注册 QML 类型
     * - 获取所需的系统服务（如 ISettings）
     * 
     * @param registry 服务注册表，用于获取和注册服务
     * @return true 初始化成功
     */
    bool initialize(mpf::ServiceRegistry* registry) override;
    
    /**
     * @brief 启动插件
     * 
     * 在此方法中：
     * - 注册导航路由
     * - 注册菜单项
     * - 加载初始数据
     * - 连接信号槽
     * 
     * @return true 启动成功
     */
    bool start() override;
    
    /**
     * @brief 停止插件
     * 
     * 在此方法中：
     * - 保存数据
     * - 断开信号连接
     * - 清理资源
     */
    void stop() override;
    
    /**
     * @brief 返回插件元数据
     * 
     * 返回的 JSON 对象应与 xxx_plugin.json 文件内容一致。
     * 主要用于运行时查询插件信息。
     */
    QJsonObject metadata() const override;
    
    /**
     * @brief 返回 QML 模块 URI
     * 
     * 【修改点5】改为你的 QML 模块 URI
     * 格式: "CompanyName.ModuleName"
     * 必须与 CMakeLists.txt 中 qt_add_qml_module 的 URI 一致
     */
    QString qmlModuleUri() const override { return "YourCo.Orders"; }
    
    /**
     * @brief 返回入口 QML 文件路径
     * 
     * 如果此插件提供应用的主入口界面，返回 QML 文件路径。
     * 大多数业务插件返回空字符串，只有"壳"插件才需要提供入口。
     * 
     * 示例: return "qrc:/YourCo/Shell/qml/Main.qml";
     */
    QString entryQml() const override { return QString(); }

private:
    /**
     * @brief 注册导航路由和菜单项
     * 
     * 使用 INavigation 注册页面路由
     * 使用 IMenu 注册侧边栏菜单项
     */
    void registerRoutes();
    
    /**
     * @brief 注册 QML 类型
     * 
     * 将 C++ 类型暴露给 QML：
     * - qmlRegisterSingletonInstance: 单例服务
     * - qmlRegisterType: 可实例化类型（如 Model）
     */
    void registerQmlTypes();

    mpf::ServiceRegistry* m_registry = nullptr;          // 服务注册表引用
    std::unique_ptr<OrdersService> m_ordersService;      // 【修改点6】业务服务实例
};

} // namespace orders
