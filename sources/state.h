#pragma once

#include "cfs.h"

#include <cereal/types/memory.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/base_class.hpp>
#include <fstream>
#include <cereal/types/vector.hpp>
#include <iostream>
#include <cereal/types/polymorphic.hpp>

/*
TODO
children是否需要改为list类型，因为vector为连续存储，当文件特别多，对象占用空间很大时怕没有连续空间放vector。
list和vector性能简单比较：https://blog.csdn.net/xxm524/article/details/86561828
这是个小型项目，children占用的内存不会特别大，所以暂时先用小内存中性能的较好vector。
*/

// 枚举文件类型：文件、目录
enum FileType
{
    File,
    Directory
};

// 状态树的基类
class StateBase
{
private:
    // 把类声明序列化函数放在private或protect域下面的时候，需要用access，这样cereal才有对该类的访问权
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(file_type, filename, mtime, file_id);
    }

private:
    // 文件类型
    FileType file_type;
    // 文件名
    std::string filename;
    // 修改时间
    std::string mtime;
    // 文件的hash
    std::string file_id;

public:
    StateBase() {}
    // 基类要有虚函数，编译器才知道用到了多态，dynamic和cereal都有用
    virtual ~StateBase() {}
    explicit StateBase(FileType type, std::string name, std::string time, std::string id) : file_type(type), filename(name), mtime(time), file_id(id) {}

    void setFiletype(FileType type)
    {
        file_type = type;
    }
    FileType getFiletype()
    {
        return file_type;
    }
    void setFilename(std::string name)
    {
        filename = name;
    }
    std::string getFilename()
    {
        return filename;
    }
    void setMtime(std::string time)
    {
        mtime = time;
    }
    std::string getMtime()
    {
        return mtime;
    }
    void setFileid(std::string id)
    {
        file_id = id;
    }
    std::string getFileid()
    {
        return file_id;
    }
};

// 文件状态
class FileState : public StateBase
{
private:
    // 把类声明序列化函数放在private或protect域下面的时候，需要用access，这样cereal才有对该类的访问权
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        // 序列化基类信息
        ar(cereal::base_class<StateBase>(this)); 
    }

public:
    FileState() {}
    explicit FileState(std::string name, std::string time = "", std::string id = "") : StateBase(File, name, time, id) {}
};

// 目录状态（状态树）
class DirectoryState : public StateBase
{
private:
    // 把类声明序列化函数放在private或protect域下面的时候，需要用access，这样cereal才有对该类的访问权
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        // 序列化基类信息
        ar(cereal::base_class<StateBase>(this), children); 

        // we have to explicitly inform the archive when it is safe to serialize
        // the deferred data - this should only be called once on the archive
        ar.serializeDeferments();
    }

private:
    // 存放当前directory的孩子节点
    std::vector<std::shared_ptr<StateBase>> children;
    // 定义别名
    typedef std::vector<std::shared_ptr<StateBase>> vector;

public:
    DirectoryState() {}
    explicit DirectoryState(std::string name, std::string time = "", std::string id = "") : StateBase(Directory, name, time, id) {}
    // explicit DirectoryState(vector ch, std::string name, std::string time = "", std::string id = "") : StateBase(Directory, name, time, id), children(ch) {}

    // void setChildren(vector ch)
    // {
    //     children = ch;
    // }
    // 获取所有孩子节点
    vector getChildren()
    {
        return children;
    }
    // 在children中添加一条记录
    void insert(std::shared_ptr<StateBase> st)
    {
        children.push_back(st);
    }
    // 将文件名为filename的记录从children中删掉
    void remove(std::string name)
    {
        for (unsigned int i = 0; i < children.size(); i++)
        {
            if (children[i]->getFilename() == name)
            {
                children.erase(children.begin() + i);
            }
        }
    }
    // 根据goal（filename, file_id）从children找出对应的对象，返回指针，找不到返回空指针
    std::shared_ptr<StateBase> findState(std::string goal)
    {
        for (unsigned int i = 0; i < children.size(); i++)
        {
            if (children[i]->getFilename() == goal || children[i]->getFileid() == goal)
            {
                return children[i];
            }
        }
        return NULL;
    }
};

// 登记用到的子类，父类可以不用登记
CEREAL_REGISTER_TYPE(FileState);
CEREAL_REGISTER_TYPE(DirectoryState);
// 登记继承关系
CEREAL_REGISTER_POLYMORPHIC_RELATION(StateBase, FileState)
CEREAL_REGISTER_POLYMORPHIC_RELATION(StateBase, DirectoryState)

// 保存状态树
void save_statetree(std::string path, std::shared_ptr<StateBase> root);

// 读取出状态树，返回StateBase多态智能指针shared_ptr
std::shared_ptr<StateBase> load_statetree(std::string path);

// 生成本地当前状态树
std::shared_ptr<StateBase> generate_statetree_local(std::string local_path);

// 生成云端当前状态树
std::shared_ptr<StateBase> generate_statetree_cloud(std::string cloud_path, std::shared_ptr<CloudFileSystem> cfs);
