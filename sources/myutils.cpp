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