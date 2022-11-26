#include "myutils.h"

#include <filesystem>


// 计算文件的 SHA256 值
std::string CalSHA256_ByFile(std::string local_path)
{
	std::string value;
	CryptoPP::SHA256 sha256;
	CryptoPP::FileSource(local_path.c_str(), true, new CryptoPP::HashFilter(sha256, new CryptoPP::HexEncoder(new CryptoPP::StringSink(value))));
	return value;
}
std::string temp_file_path = std::filesystem::current_path().string() + "/cloudsync_temp_file";
// 计算云端文件的 SHA256 值（先下载到本地，再计算hash）
std::string CalSHA256_ByCloudFile(std::string bucket_name, std::string cloud_path, std::shared_ptr<OssClient> client)
{
	GetObjectRequest request(bucket_name, cloud_path);
    request.setResponseStreamFactory([=]()
                                     { return std::make_shared<std::fstream>(temp_file_path, std::ios_base::out | std::ios_base::in | std::ios_base::trunc | std::ios_base::binary); });

    auto outcome = client->GetObject(request);
	
	std::string value;
	CryptoPP::SHA256 sha256;
	CryptoPP::FileSource(temp_file_path.c_str(), true, new CryptoPP::HashFilter(sha256, new CryptoPP::HexEncoder(new CryptoPP::StringSink(value))));
	return value;
}

// 计算数据的 SHA256 值
// 这里的byte要用CryptoPP作用域，因为std作用域中的是个类，不是unsigned char
std::string CalSHA256_ByMem(const CryptoPP::byte *data, size_t length)
{
	std::string value;
	CryptoPP::SHA256 sha256;
	CryptoPP::StringSource(data, length, true, new CryptoPP::HashFilter(sha256, new CryptoPP::HexEncoder(new CryptoPP::StringSink(value))));
	return value;
}

// 取出路径中的文件名（目录名）
// 情况1:a/b.txt    结果：b.txt
// 情况2:a/c/       结果：c/
std::string GetFileName(std::string path){
	std::string result;
	// 如果是目录的话，先去掉最后面的/
	if(path[path.size()-1] == '/')
	{
		path.pop_back();
		int pos = path.find_last_of('/');
		result = path.substr(pos + 1, path.size());
		result.push_back('/');
	}
	// 如果是文件
	else
	{
		int pos = path.find_last_of('/');
		result = path.substr(pos + 1, path.size());
	}

	return result;
}

// unix下的获取当前时间方法（logger用）
void get_current_time(timespec& current_time)
{
    clock_gettime(CLOCK_REALTIME, &current_time);
}

void get_current_time_in_tm(struct tm* tm, int* nanoseconds)
{
    timespec now;
    get_current_time(now);
    if (tm)
        gmtime_r(&now.tv_sec, tm);
    if (nanoseconds)
        *nanoseconds = static_cast<int>(now.tv_nsec);
}
