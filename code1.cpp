#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

// 工具函数：生成唯一ID
string generateID() {
    static int idCounter = 1000;
    return "U" + to_string(idCounter++);
}

// 工具函数：获取当前时间
string getCurrentTime() {
    time_t now = time(0);
    struct tm ltm;
    localtime_s(&ltm, &now);
    stringstream ss;
    ss << 1900 + ltm.tm_year << "-"
        << setfill('0') << setw(2) << 1 + ltm.tm_mon << "-"
        << setfill('0') << setw(2) << ltm.tm_mday << " "
        << setfill('0') << setw(2) << ltm.tm_hour << ":"
        << setfill('0') << setw(2) << ltm.tm_min;
    return ss.str();
}

// 评价类
class Review {
public:
    string reviewID;
    string transactionID;
    string reviewerID;
    int score; // 1-5分
    string content;
    string createTime;

    Review(string tid, string rid, int s, string cnt)
        : transactionID(tid), reviewerID(rid), score(s), content(cnt), createTime(getCurrentTime()) {
        reviewID = "R" + generateID().substr(1);
    }
};

// 用户类
class User {
protected:
    string name;
    string userID;
    int creditScore; // 初始80分
    string memberLevel;
    int transactionCount;
    int returnCount;
    double transactionSuccessRate;

public:
    User(string n) : name(n), userID(generateID()), creditScore(80),
        transactionCount(0), returnCount(0), transactionSuccessRate(100.0) {
        updateMemberLevel();
    }

    // 更新会员等级
    void updateMemberLevel() {
        if (creditScore >= 100) memberLevel = "高级会员";
        else if (creditScore >= 80) memberLevel = "普通会员";
        else if (creditScore >= 60) memberLevel = "降级预警会员";
        else memberLevel = "限制会员";
    }

    // 更新信用积分
    void updateCreditScore(int change) {
        creditScore += change;
        if (creditScore < 0) creditScore = 0;
        if (creditScore > 150) creditScore = 150;
        updateMemberLevel();
    }

    // 更新交易记录
    void updateTransactionRecord(bool isSuccess, bool isReturn) {
        transactionCount++;
        if (!isSuccess) transactionSuccessRate = (transactionCount - 1) * 100.0 / transactionCount;
        if (isReturn) returnCount++;
    }

    // 获取商家视角的用户交易表现（修复字符串拼接错误）
    string getTransactionPerformance() const {
        stringstream ss;
        ss << fixed << setprecision(1);
        ss << "交易次数：" << transactionCount
            << " | 退货次数：" << returnCount
            << " | 交易成功率：" << transactionSuccessRate << "%";
        return ss.str();
    }

    //  getter方法
    string getName() const { return name; }
    string getUserID() const { return userID; }
    int getCreditScore() const { return creditScore; }
    string getMemberLevel() const { return memberLevel; }
    double getSuccessRate() const { return transactionSuccessRate; }
};

// 商户类（继承用户类）
class Merchant : public User {
private:
    string qualification; // 商户资质
    vector<string> productIDs; // 关联的商品ID

public:
    Merchant(string n, string q) : User(n), qualification(q) {}

    void addProductID(string pid) { productIDs.push_back(pid); }
    void removeProductID(string pid) {
        productIDs.erase(remove(productIDs.begin(), productIDs.end(), pid), productIDs.end());
    }

    string getQualification() const { return qualification; }
    vector<string> getProductIDs() const { return productIDs; }
    int getProductCount() const { return productIDs.size(); } // 获取商品数量
};

// 商品类
class Product {
public:
    string productID;
    string name;
    double price;
    string version;
    int stock;
    string category;
    string merchantID; // 关联商户ID

    Product(string n, double p, string v, int s, string c, string mid)
        : name(n), price(p), version(v), stock(s), category(c), merchantID(mid) {
        productID = "P" + generateID().substr(1);
    }
};

// 交易类
class Transaction {
public:
    string transactionID;
    string buyerID;
    string sellerID;
    string productID;
    string status; // 待联系、已完成、已取消
    string contactExchangeTime;
    string buyerContact;
    string sellerContact;

    Transaction(string bid, string sid, string pid)
        : buyerID(bid), sellerID(sid), productID(pid), status("待联系") {
        transactionID = "T" + generateID().substr(1);
    }

    // 交换联系方式
    void exchangeContacts(string bContact, string sContact) {
        buyerContact = bContact;
        sellerContact = sContact;
        contactExchangeTime = getCurrentTime();
        status = "已联系";
    }

    void completeTransaction() { status = "已完成"; }
    void cancelTransaction() { status = "已取消"; }

    //  getter方法
    string getTransactionID() const { return transactionID; }
    string getStatus() const { return status; }
    string getBuyerContact() const { return buyerContact; }
    string getSellerContact() const { return sellerContact; }
};

// 网络商场系统核心类
class OnlineMallSystem {
private:
    vector<User> users;
    vector<Merchant> merchants;
    vector<Product> products;
    vector<Transaction> transactions;
    vector<Review> reviews;

    // 查找用户索引
    int findUserIndex(string userID) {
        for (int i = 0; i < users.size(); i++) {
            if (users[i].getUserID() == userID) return i;
        }
        return -1;
    }

    // 查找商户索引
    int findMerchantIndex(string merchantID) {
        for (int i = 0; i < merchants.size(); i++) {
            if (merchants[i].getUserID() == merchantID) return i;
        }
        return -1;
    }

    // 查找商品索引
    int findProductIndex(string productID) {
        for (int i = 0; i < products.size(); i++) {
            if (products[i].productID == productID) return i;
        }
        return -1;
    }

    // 查找交易索引
    int findTransactionIndex(string transactionID) {
        for (int i = 0; i < transactions.size(); i++) {
            if (transactions[i].getTransactionID() == transactionID) return i;
        }
        return -1;
    }

    // 获取商户信息
    Merchant* getMerchant(string merchantID) {
        int idx = findMerchantIndex(merchantID);
        return idx != -1 ? &merchants[idx] : nullptr;
    }

public:
    // 用户注册
    string registerUser(string userName) {
        users.emplace_back(userName);
        cout << "用户注册成功！" << endl;
        cout << "用户名：" << userName << " | 用户ID：" << users.back().getUserID()
            << " | 初始信用积分：" << users.back().getCreditScore()
            << " | 会员等级：" << users.back().getMemberLevel() << endl;
        cout << "提示：普通用户不能添加商品，如需卖商品请选择【2. 创建商户】" << endl;
        return users.back().getUserID();
    }

    // 创建商户
    string createMerchant(string userName, string qualification) {
        merchants.emplace_back(userName, qualification);
        cout << "商户创建成功！" << endl;
        cout << "商户名：" << userName << " | 商户ID：" << merchants.back().getUserID()
            << " | 资质：" << qualification
            << " | 初始信用积分：" << merchants.back().getCreditScore() << endl;
        return merchants.back().getUserID();
    }

    // 商户添加商品
    bool addProduct(string merchantID, string productName, double price, string version, int stock, string category) {
        // 先检查是否是普通用户
        int uIdx = findUserIndex(merchantID);
        if (uIdx != -1) {
            cout << "错误：ID为" << merchantID << "的账号是普通用户，不是商户！" << endl;
            cout << "请先使用【2. 创建商户】功能注册为商户" << endl;
            return false;
        }

        int mIdx = findMerchantIndex(merchantID);
        if (mIdx == -1) {
            cout << "商户不存在，添加商品失败！" << endl;
            return false;
        }
        products.emplace_back(productName, price, version, stock, category, merchantID);
        merchants[mIdx].addProductID(products.back().productID);
        cout << "商品添加成功！" << endl;
        cout << "商品ID：" << products.back().productID << " | 商品名称：" << productName
            << " | 价格：" << price << " | 版本：" << version << " | 库存：" << stock << endl;
        return true;
    }

    // 搜索商品（按分类）
    vector<Product> searchProducts(string category) {
        vector<Product> result;
        for (auto& p : products) {
            if (p.category == category) result.push_back(p);
        }
        // 按商户信用积分排序（降序）
        sort(result.begin(), result.end(), [this](Product& a, Product& b) {
            int creditA = getMerchant(a.merchantID)->getCreditScore();
            int creditB = getMerchant(b.merchantID)->getCreditScore();
            return creditA > creditB;
            });
        return result;
    }

    // 搜索用户（按名称）
    vector<User> searchUsers(string name) {
        vector<User> result;
        for (auto& u : users) {
            // 部分匹配搜索
            if (u.getName().find(name) != string::npos) {
                result.push_back(u);
            }
        }
        // 按信用积分排序
        sort(result.begin(), result.end(), [](User& a, User& b) {
            return a.getCreditScore() > b.getCreditScore();
            });
        return result;
    }

    // 搜索商户（按名称或资质）
    vector<Merchant> searchMerchants(string keyword) {
        vector<Merchant> result;
        for (auto& m : merchants) {
            // 匹配名称或资质
            if (m.getName().find(keyword) != string::npos ||
                m.getQualification().find(keyword) != string::npos) {
                result.push_back(m);
            }
        }
        // 按信用积分排序
        sort(result.begin(), result.end(), [](Merchant& a, Merchant& b) {
            return a.getCreditScore() > b.getCreditScore();
            });
        return result;
    }

    // 同类商品对比
    void compareProducts(string category) {
        vector<Product> sameCategory = searchProducts(category);
        if (sameCategory.empty()) {
            cout << "该分类下无商品可对比！" << endl;
            return;
        }
        cout << "\n=== 同类商品对比（" << category << "）===" << endl;
        cout << setw(8) << "商品ID" << setw(15) << "商品名称" << setw(10) << "价格"
            << setw(10) << "版本" << setw(15) << "商户信用" << setw(10) << "商户名" << endl;
        for (auto& p : sameCategory) {
            Merchant* merchant = getMerchant(p.merchantID);
            cout << setw(8) << p.productID << setw(15) << p.name << setw(10) << p.price
                << setw(10) << p.version << setw(15) << merchant->getCreditScore()
                << setw(10) << merchant->getName() << endl;
        }
    }

    // 创建交易（交换联系方式）
    string createTransaction(string buyerID, string productID, string buyerContact) {
        int uIdx = findUserIndex(buyerID);
        int pIdx = findProductIndex(productID);
        if (uIdx == -1 || pIdx == -1) {
            cout << "用户或商品不存在，创建交易失败！" << endl;
            return "";
        }
        Product& product = products[pIdx];
        Merchant* merchant = getMerchant(product.merchantID);
        if (!merchant) {
            cout << "商户不存在，创建交易失败！" << endl;
            return "";
        }

        // 检查库存
        if (product.stock <= 0) {
            cout << "商品库存不足，创建交易失败！" << endl;
            return "";
        }

        // 创建交易
        transactions.emplace_back(buyerID, product.merchantID, productID);
        Transaction& t = transactions.back();

        // 商户输入联系方式
        string sellerContact;
        cout << "请商户" << merchant->getName() << "输入联系方式：";
        cin >> sellerContact;
        t.exchangeContacts(buyerContact, sellerContact);

        // 减少库存
        product.stock--;

        cout << "\n交易创建成功！联系方式已交换" << endl;
        cout << "交易ID：" << t.getTransactionID() << endl;
        cout << "买家联系方式：" << t.getBuyerContact() << endl;
        cout << "卖家联系方式：" << t.getSellerContact() << endl;
        return t.getTransactionID();
    }

    // 完成交易并提交评价
    void completeTransactionAndReview(string transactionID, int score, string content) {
        int tIdx = findTransactionIndex(transactionID);
        if (tIdx == -1) {
            cout << "交易不存在！" << endl;
            return;
        }
        Transaction& t = transactions[tIdx];
        if (t.getStatus() != "已联系") {
            cout << "交易状态不允许提交评价！" << endl;
            return;
        }

        // 完成交易
        t.completeTransaction();

        // 提交评价
        reviews.emplace_back(transactionID, t.buyerID, score, content);

        // 更新买家和卖家信用积分
        int buyerIdx = findUserIndex(t.buyerID);
        Merchant* seller = getMerchant(t.sellerID);
        users[buyerIdx].updateCreditScore(1); // 买家完成交易+1分
        seller->updateCreditScore(score - 3); // 评价得分映射为信用变化

        // 更新交易记录
        users[buyerIdx].updateTransactionRecord(true, false);
        seller->updateTransactionRecord(true, false);

        cout << "交易完成！评价提交成功" << endl;
        cout << "评价内容：" << content << " | 评分：" << score << "分" << endl;
        cout << "买家信用积分更新为：" << users[buyerIdx].getCreditScore() << endl;
        cout << "卖家信用积分更新为：" << seller->getCreditScore() << endl;
    }

    // 商家查看用户交易表现
    void merchantViewUserPerformance(string merchantID, string userID) {
        int mIdx = findMerchantIndex(merchantID);
        int uIdx = findUserIndex(userID);
        if (mIdx == -1 || uIdx == -1) {
            cout << "商户或用户不存在！" << endl;
            return;
        }
        cout << "\n=== 用户" << users[uIdx].getName() << "的交易表现 ===" << endl;
        cout << users[uIdx].getTransactionPerformance() << endl;
    }

    // 显示会员等级
    void showMemberLevel(string userID) {
        int uIdx = findUserIndex(userID);
        if (uIdx != -1) {
            cout << "用户" << users[uIdx].getName() << " | 会员等级：" << users[uIdx].getMemberLevel()
                << " | 信用积分：" << users[uIdx].getCreditScore() << endl;
        }
        else {
            Merchant* merchant = getMerchant(userID);
            if (merchant) {
                cout << "商户" << merchant->getName() << " | 会员等级：" << merchant->getMemberLevel()
                    << " | 信用积分：" << merchant->getCreditScore() << endl;
            }
            else {
                cout << "用户/商户不存在！" << endl;
            }
        }
    }

    // 显示搜索结果
    void displaySearchResults(const vector<Product>& results) {
        if (results.empty()) {
            cout << "该分类下无商品！" << endl;
            return;
        }
        cout << "\n=== 商品搜索结果 ===" << endl;
        cout << setw(8) << "商品ID" << setw(15) << "商品名称" << setw(10) << "价格"
            << setw(15) << "商户名" << setw(15) << "商户信用" << endl;
        for (const auto& p : results) {
            Merchant* merchant = getMerchant(p.merchantID);
            cout << setw(8) << p.productID << setw(15) << p.name << setw(10) << p.price
                << setw(15) << merchant->getName()
                << setw(15) << merchant->getCreditScore() << endl;
        }
    }

    // 显示用户搜索结果
    void displayUserSearchResults(const vector<User>& results) {
        if (results.empty()) {
            cout << "未找到匹配的用户！" << endl;
            return;
        }
        cout << "\n=== 用户搜索结果 ===" << endl;
        cout << setw(8) << "用户ID" << setw(15) << "用户名" << setw(15) << "会员等级"
            << setw(15) << "信用积分" << setw(15) << "交易成功率" << endl;
        for (const auto& u : results) {
            cout << setw(8) << u.getUserID() << setw(15) << u.getName() << setw(15) << u.getMemberLevel()
                << setw(15) << u.getCreditScore()
                << setw(15) << fixed << setprecision(1) << u.getSuccessRate() << "%" << endl;
        }
    }

    // 显示商户搜索结果
    void displayMerchantSearchResults(const vector<Merchant>& results) {
        if (results.empty()) {
            cout << "未找到匹配的商户！" << endl;
            return;
        }
        cout << "\n=== 商户搜索结果 ===" << endl;
        cout << setw(8) << "商户ID" << setw(15) << "商户名" << setw(15) << "资质"
            << setw(15) << "信用积分" << setw(15) << "商品数量" << endl;
        for (const auto& m : results) {
            cout << setw(8) << m.getUserID() << setw(15) << m.getName() << setw(15) << m.getQualification()
                << setw(15) << m.getCreditScore()
                << setw(15) << m.getProductCount() << endl;
        }
    }
};

// 命令行交互界面
void showMenu() {
    cout << "\n==================== 网络商场系统 ====================" << endl;
    cout << "1. 用户注册(买家)    2. 创建商户(卖家)    3. 商户添加商品" << endl;
    cout << "4. 搜索商品          5. 搜索用户          6. 搜索商户" << endl;
    cout << "7. 商品对比          8. 创建交易(交换联系方式)  9. 完成交易+评价" << endl;
    cout << "10. 商家查看用户表现  11. 查看会员等级     0. 退出系统" << endl;
    cout << "======================================================\n";
    cout << "提示：买家使用1注册，卖家需使用2创建商户账号\n";
    cout << "请选择操作：";
}

int main() {
    OnlineMallSystem mall;
    int choice;
    string userID, merchantID, productID, name, qualification, category, contact, content, keyword;
    double price;
    string version;
    int stock, score;

    while (true) {
        showMenu();
        cin >> choice;
        switch (choice) {
        case 1:
            cout << "请输入用户名(买家)：";
            cin >> name;
            mall.registerUser(name);
            break;
        case 2:
            cout << "请输入商户名(卖家)：";
            cin >> name;
            cout << "请输入商户资质：";
            cin >> qualification;
            mall.createMerchant(name, qualification);
            break;
        case 3:
            cout << "请输入商户ID(卖家)：";
            cin >> merchantID;
            cout << "请输入商品名称：";
            cin >> name;
            cout << "请输入商品价格：";
            cin >> price;
            cout << "请输入商品版本：";
            cin >> version;
            cout << "请输入商品库存：";
            cin >> stock;
            cout << "请输入商品分类：";
            cin >> category;
            mall.addProduct(merchantID, name, price, version, stock, category);
            break;
        case 4:
            cout << "请输入商品分类搜索关键词：";
            cin >> category;
            {
                vector<Product> res = mall.searchProducts(category);
                mall.displaySearchResults(res);
            }
            break;
        case 5:  // 搜索用户
            cout << "请输入用户名搜索关键词：";
            cin >> keyword;
            {
                vector<User> userRes = mall.searchUsers(keyword);
                mall.displayUserSearchResults(userRes);
            }
            break;
        case 6:  // 搜索商户
            cout << "请输入商户搜索关键词(名称或资质)：";
            cin >> keyword;
            {
                vector<Merchant> merchantRes = mall.searchMerchants(keyword);
                mall.displayMerchantSearchResults(merchantRes);
            }
            break;
        case 7:
            cout << "请输入对比分类：";
            cin >> category;
            mall.compareProducts(category);
            break;
        case 8:
            cout << "请输入买家ID：";
            cin >> userID;
            cout << "请输入商品ID：";
            cin >> productID;
            cout << "请输入买家联系方式：";
            cin >> contact;
            mall.createTransaction(userID, productID, contact);
            break;
        case 9:
            cout << "请输入交易ID：";
            cin >> productID;
            cout << "请输入评价分数（1-5）：";
            cin >> score;
            cin.ignore();
            cout << "请输入评价内容：";
            getline(cin, content);
            mall.completeTransactionAndReview(productID, score, content);
            break;
        case 10:
            cout << "请输入商户ID：";
            cin >> merchantID;
            cout << "请输入要查看的用户ID：";
            cin >> userID;
            mall.merchantViewUserPerformance(merchantID, userID);
            break;
        case 11:
            cout << "请输入用户/商户ID：";
            cin >> userID;
            mall.showMemberLevel(userID);
            break;
        case 0:
            cout << "退出系统成功！" << endl;
            return 0;
        default:
            cout << "无效操作，请重新选择！" << endl;
            break;
        }
        cout << "\n按任意键继续...";
        cin.ignore();
        cin.get();
    }
}