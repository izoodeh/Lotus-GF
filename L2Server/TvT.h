#ifndef TVT_H
#define TVT_H
#include "TvTMatch.h"

namespace TvT
{
	class CTvT : public CFeature
	{
	private:
		BOOL m_enabled;
		list<TvT::TvTInfo*> m_tvtInfo;
		TvT::CMatch* m_lpEvent;
		vector<UINT> m_inviteMessageId;

		void LoadINI();
	public:
		CTvT();
		~CTvT();
		void Init();
		void TimerExpired();
		void OnExit(User *pUser, bool  useLock = true);
		void Register(User *pUser, int team);
		void Unregister(User *pUser);
		void OnDie(User *pUser);
		void OnNpcSpawn(CNpc *pNpc);
		void TimerExpired(User *pUser);
		bool CanUse(CItem *pItem);
		void ValidateItems(User *pUser);
		bool IsSkillBlocked(INT32 skillId);
		bool HandleReply(User *pUser, UINT msgId, UINT reply);
	};
};

extern TvT::CTvT g_TvT;

#endif
