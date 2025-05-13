/**
 * @file fifo_test.cpp
 * @brief 环形队列的测试用例
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "siso_fifo.h"

// 测试因子：
// 1. 参数是否合适：
// 1.1 各种NULL
// 1.2 申请0
// 1.3 申请大于总容量

// 2. 操作是否允许
// 2.1 上一次读尚未结束，下一次读就到来了
// 2.2 上一次写尚未结束，下一次写就到来了

// 3. 读写是否截断
// 4. 读写是否位于边界条件

// 测试方式：
// 1和2都是一票否决的，所以单独测试
// 3和4组合测试，共4组测试用例

// 测试夹具
class RcsFifoTest : public ::testing::Test {
protected:
    RcsFifoHandle_t handle;
    RcsFifo_t fifo;
    static constexpr size_t fifoSize = 16;

    void SetUp() override {
        fifo = RcsFifoCreate(fifoSize);
        handle = *((RcsFifoHandle_t*)fifo);
    }

    void TearDown() override {
        RcsFifoDestroy(fifo);
    }
};

// 创建实例测试
TEST_F(RcsFifoTest, CreateValidFifo) 
{
    ASSERT_NE(fifo, nullptr);
    EXPECT_EQ(handle.memSize, fifoSize);
    EXPECT_EQ(handle.indexReadHead, 0u);
    EXPECT_EQ(handle.indexWriteTail, 0u);
}

// 参数不合适测试
TEST_F(RcsFifoTest, InvalidParam) 
{
    void* memAcquired[2] = {nullptr};

    int ret = RcsFifoSendAcquire(fifo, 0, memAcquired); // 申请0
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    ret = RcsFifoSendAcquire(fifo, fifoSize, memAcquired); // 申请大于总容量
    ASSERT_EQ(ret, RCS_FIFO_NO_SPACE); // 应失败

    ret = RcsFifoSendAcquire(NULL, fifoSize, memAcquired); // 申请NULL
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    ret = RcsFifoSendAcquire(fifo, fifoSize, NULL); // 申请NULL
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    ret = RcsFifoSendAcquireNoSplit(fifo, 0, memAcquired); // 申请0
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    ret = RcsFifoSendAcquireNoSplit(fifo, fifoSize, memAcquired); // 申请大于总容量
    ASSERT_EQ(ret, RCS_FIFO_NO_SPACE); // 应失败

    ret = RcsFifoSendAcquireNoSplit(NULL, fifoSize, memAcquired); // 申请NULL
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    ret = RcsFifoSendAcquireNoSplit(fifo, fifoSize, NULL); // 申请NULL
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    ret = RcsFifoRecvAcquire(fifo, 0, memAcquired); // 申请0
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    ret = RcsFifoRecvAcquire(fifo, fifoSize, memAcquired); // 申请大于总容量
    ASSERT_EQ(ret, RCS_FIFO_NO_DATA); // 应失败

    ret = RcsFifoRecvAcquire(NULL, fifoSize, memAcquired); // 申请NULL
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    ret = RcsFifoRecvAcquire(fifo, fifoSize, NULL); // 申请NULL
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    ret = RcsFifoRecvAcquireNoSplit(fifo, 0, memAcquired); // 申请0
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    ret = RcsFifoRecvAcquireNoSplit(fifo, fifoSize, memAcquired); // 申请大于总容量
    ASSERT_EQ(ret, RCS_FIFO_NO_DATA); // 应失败

    ret = RcsFifoRecvAcquireNoSplit(NULL, fifoSize, memAcquired); // 申请NULL
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    ret = RcsFifoRecvAcquireNoSplit(fifo, fifoSize, NULL); // 申请NULL
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    ret = RcsFifoSendComplete(fifo,NULL);
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    ret = RcsFifoRecvComplete(fifo,NULL);
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    void *memInvalid[2] = {nullptr};
    ret = RcsFifoSendComplete(fifo,(const void**)memInvalid);
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败

    ret = RcsFifoRecvComplete(fifo,(const void**)memInvalid);
    ASSERT_EQ(ret, RCS_FIFO_INVALID_PARAM); // 应失败
}

// 操作是否允许测试
TEST_F(RcsFifoTest,OperationAllowed) 
{

    // 上一次写没有结束，就要再写一次
    void* memAcquired[2] = {nullptr};
    int ret = RcsFifoSendAcquire(fifo, 3, memAcquired);
    ASSERT_EQ(ret,3);
    ret = RcsFifoSendAcquire(fifo,3,memAcquired);
    ASSERT_EQ(ret,RCS_FIFO_NOT_ALLOWED);
    ret = RcsFifoSendComplete(fifo,(const void**)memAcquired);
    ASSERT_EQ(ret,RCS_FIFO_OK);

    // 上一次读还没结束，就要再读一次
    ret = RcsFifoRecvAcquire(fifo, 3, memAcquired);
    ASSERT_EQ(ret,3);
    ret = RcsFifoRecvAcquire(fifo,3,memAcquired);
    ASSERT_EQ(ret,RCS_FIFO_NOT_ALLOWED);
    ret = RcsFifoRecvComplete(fifo,(const void**)memAcquired);
    ASSERT_EQ(ret,RCS_FIFO_OK);
}

// 不截断+不边界 读写测试
TEST_F(RcsFifoTest, Loopback_NoTrunc_NoBoundary) 
{
    void* memAcquired[2] = {nullptr};

    const char data[] = "hello";
    int ret = RcsFifoSendAcquire(fifo, sizeof(data), memAcquired);
    ASSERT_EQ(ret, sizeof(data));
    memcpy(memAcquired[0], data, sizeof(data));
    ASSERT_EQ(RcsFifoSendComplete(fifo, (const void**)memAcquired), 0);

    ret = RcsFifoRecvAcquire(fifo, sizeof(data), memAcquired);
    ASSERT_EQ(ret, sizeof(data));
    EXPECT_STREQ((char*)memAcquired[0], data);
    ASSERT_EQ(RcsFifoRecvComplete(fifo, (const void**)memAcquired), 0);
}

// 截断+不边界 读写测试
TEST_F(RcsFifoTest, Loopback_Trunc_NoBoundary) 
{
    void* txBlk[2] = {nullptr}, *rxBlk[2] = {nullptr};
    const int txLen =5;

    // 生成测试数据
    std::vector<char> txData(txLen);
    memcpy(txData.data(), "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ", txData.size());

    // 写入
    int ret = RcsFifoSendAcquire(fifo, txData.size(), txBlk);
    ASSERT_EQ(ret, txLen) << "第一段长度应为 txLen";
    ASSERT_EQ(txBlk[1], nullptr) << "应不触发跨界，第二段指针应为空";
    size_t firstLen = ret, secondLen = txData.size() - firstLen;
    memcpy(txBlk[0], txData.data(), firstLen);
    memcpy(txBlk[1], txData.data() + firstLen, secondLen);
    ASSERT_EQ(RcsFifoSendComplete(fifo, (const void**)txBlk), RCS_FIFO_OK);

    // 读取
    ret = RcsFifoRecvAcquire(fifo, txData.size(), rxBlk);
    ASSERT_EQ(ret, txLen) << "第一段读取长度应为 txLen";
    ASSERT_EQ(rxBlk[1], nullptr);
    size_t rxFirst = ret, rxSecond = txData.size() - rxFirst;

    // 分段校验
    EXPECT_TRUE(std::equal(txData.begin(), txData.begin()+rxFirst,
                           reinterpret_cast<char*>(rxBlk[0])));

    // 完成读取
    ASSERT_EQ(RcsFifoRecvComplete(fifo, (const void**)rxBlk), RCS_FIFO_OK);
}

// 不截断+边界 写测试
TEST_F(RcsFifoTest, Write_NoTrunc_Boundary) 
{
    void* memAcquired[2];
    int ret = RcsFifoSendAcquire(fifo, fifoSize - 1, memAcquired); // 申请等于总容量
    EXPECT_EQ(ret,fifoSize - 1); // 应成功
    RcsFifoSendComplete(fifo, (const void**)memAcquired);

    ret = RcsFifoSendAcquire(fifo, 1, memAcquired); // 申请大于总容量
    EXPECT_EQ(ret, RCS_FIFO_NO_SPACE); // 应失败
}

// 不截断+边界 读测试
TEST_F(RcsFifoTest, ReadRead_NoTrunc_Boundary) 
{
    //先给一点数据
    char testStr[] = "Hello";
    void* memAcquired[2];
    int ret = RcsFifoSendAcquire(fifo, sizeof(testStr), memAcquired); 
    EXPECT_EQ(ret,sizeof(testStr)); 
    RcsFifoSendComplete(fifo, (const void**)memAcquired);

    ret = RcsFifoRecvAcquire(fifo, sizeof(testStr), memAcquired); // 申请等于总容量
    EXPECT_EQ(ret,sizeof(testStr)); // 应成功
    RcsFifoRecvComplete(fifo, (const void**)memAcquired);

    ret = RcsFifoRecvAcquire(fifo, 1, memAcquired); // 申请大于总容量
    EXPECT_EQ(ret, RCS_FIFO_NO_DATA); // 应失败
}

// 截断+边界 读写测试
TEST_F(RcsFifoTest, Loopback_Trunc_Boundary) 
{
    void* txBlk[2] = {nullptr}, *rxBlk[2] = {nullptr};
    const int bias = 3; // 第一次申请 fifoSize-bias，剩余连续空间 bias，会触发 wrap-around

    // 1 推进 head 到 尾部-偏移
    int ret = RcsFifoSendAcquire(fifo, fifoSize - bias, txBlk);
    ASSERT_EQ(ret, fifoSize - bias);
    ASSERT_EQ(RcsFifoSendComplete(fifo, (const void**)txBlk), RCS_FIFO_OK);
    ret = RcsFifoRecvAcquire(fifo, fifoSize - bias, rxBlk);
    ASSERT_EQ(ret, fifoSize - bias);
    ASSERT_EQ(RcsFifoRecvComplete(fifo, (const void**)rxBlk), RCS_FIFO_OK);

    // 2 生成 15 字节测试数据
    std::vector<char> txData(fifoSize - 1);
    memcpy(txData.data(), "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ", txData.size());

    // 3 跨界写入
    ret = RcsFifoSendAcquire(fifo, txData.size(), txBlk);
    ASSERT_EQ(ret, bias) << "第一段长度应为 bias";
    ASSERT_NE(txBlk[1], nullptr) << "应触发跨界，第二段指针不应为空";
    size_t firstLen = ret, secondLen = txData.size() - firstLen;
    memcpy(txBlk[0], txData.data(), firstLen);
    memcpy(txBlk[1], txData.data() + firstLen, secondLen);
    ASSERT_EQ(RcsFifoSendComplete(fifo, (const void**)txBlk), RCS_FIFO_OK);

    // 4 跨界读取
    ret = RcsFifoRecvAcquire(fifo, txData.size(), rxBlk);
    ASSERT_EQ(ret, bias) << "第一段读取长度应为 bias";
    ASSERT_NE(rxBlk[1], nullptr);
    size_t rxFirst = ret, rxSecond = txData.size() - rxFirst;

    // 5 分段校验
    EXPECT_TRUE(std::equal(txData.begin(), txData.begin()+rxFirst,
                           reinterpret_cast<char*>(rxBlk[0])));
    EXPECT_TRUE(std::equal(txData.begin()+rxFirst, txData.end(),
                           reinterpret_cast<char*>(rxBlk[1])));

    // 6 拼装并整体验证
    std::vector<char> rxData;
    rxData.insert(rxData.end(), 
                  reinterpret_cast<char*>(rxBlk[0]), 
                  reinterpret_cast<char*>(rxBlk[0]) + rxFirst);
    rxData.insert(rxData.end(), 
                  reinterpret_cast<char*>(rxBlk[1]), 
                  reinterpret_cast<char*>(rxBlk[1]) + rxSecond);
    EXPECT_EQ(rxData, txData);

    // 7 完成读取
    ASSERT_EQ(RcsFifoRecvComplete(fifo, (const void**)rxBlk), RCS_FIFO_OK);
}



