#ifndef TextureMgr_H
#define TextureMgr_H

#include <memory>
#include <string>

namespace byhj
{
	namespace d3d
	{
		class TextureMgr
		{
		public:
			TextureMgr() = default;
			~TextureMgr() = default;
			static std::shared_ptr<TextureMgr> getInstance()
			{
				static std::shared_ptr<TextureMgr> pInstance = std::make_shared<TextureMgr>();

				return pInstance;
			}

			void setDir(const std::string &dir)
			{
				m_Dir = dir;
			}
			std::string getDir() const
			{
				return m_Dir;
			}
		private:
			std::string m_Dir = "../../media/textures/";
		};
	}
}
#endif // TextureMgr_H
