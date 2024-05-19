#include <Zut/ZxFs.h>
#include <format>
#include <stdexcept>


namespace Zqf::Zut::ZxFs
{
	Walk::Walk(const std::string_view msWalkDir)
	{
		if (msWalkDir[msWalkDir.size() - 1] != '/') { throw std::runtime_error(std::format("ZxPath::Walk : dir {} format error!", msWalkDir)); }

		memcpy(m_aSearchDir, msWalkDir.data(), msWalkDir.size());
		m_aSearchDir[msWalkDir.size() + 0] = '*';
		m_aSearchDir[msWalkDir.size() + 1] = '\0';
		m_msSearchDir = { m_aSearchDir, msWalkDir.size() + 1 };

		if (auto hfind_opt = ZxNative::Fs::WalkCreate(m_msSearchDir)) { m_hFind = *hfind_opt; }

		throw std::runtime_error(std::format("ZxPath::Walk: find dir {} first file error! ", m_msSearchDir));
	}

	Walk::~Walk()
	{
		ZxNative::Fs::WalkClose(m_hFind);
	}

	auto Walk::GetPath() -> std::string_view
	{
		memmove(m_aFindBuffer + m_msSearchDir.size() - 1, m_aFindBuffer, m_msFindName.size() + 1);
		m_msFindName = { m_aFindBuffer + m_msSearchDir.size() - 1, m_msFindName.size() };
		memcpy(m_aFindBuffer, m_msSearchDir.data(), m_msSearchDir.size() - 1);
		return { m_aFindBuffer, m_msFindName.size() + m_msSearchDir.size() - 1 };
	}

	auto Walk::GetName() const -> std::string_view
	{
		return m_msFindName;
	}

	auto Walk::NextDir() -> bool
	{
		if (const auto opt_dir = ZxNative::Fs::WalkNextDir(m_hFind, m_aFindBuffer))
		{
			m_msFindName = *opt_dir;
			return true;
		}
		return false;
	}

	auto Walk::NextFile() -> bool
	{
		if (const auto opt_file = ZxNative::Fs::WalkNextFile(m_hFind, m_aFindBuffer))
		{
			m_msFindName = *opt_file;
			return true;
		}
		return false;
	}

	auto Walk::IsSuffix(const std::string_view u8Suffix) const -> bool
	{
		return ZxFs::Suffix(m_msFindName) == u8Suffix ? true : false;
	}


	auto Suffix(const std::string_view msPath) -> std::string_view
	{
		if (msPath.empty()) { return { "", 0 }; }

		for (auto ite = std::rbegin(msPath); ite != std::rend(msPath); ite++)
		{
			if (*ite == '/')
			{
				break;
			}
			else if (*ite == '.')
			{
				return msPath.substr(std::distance(ite, std::rend(msPath)) - 1);
			}
		}

		return { "", 0 };
	}

	auto FileName(const std::string_view msPath) -> std::string_view
	{
		if (msPath.empty()) { return { "", 0 }; }
		if (msPath.back() == '/') { return { "", 0 }; }
		auto pos = msPath.rfind('/');
		return pos != std::string_view::npos ? msPath.substr(pos + 1) : std::string_view{ "", 0 };
	}
}