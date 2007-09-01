//=================================================================================================
//
//   File : kvi_regusersdb.cpp
//   Creation date : Sat Sep 09 2000 15:46:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=================================================================================================




#include "kvi_debug.h"

#define _KVI_REGUSERDB_CPP_
#include "kvi_regusersdb.h"

#include "kvi_config.h"
#include "kvi_locale.h"
#include "kvi_stringconversion.h"

/*
	@doc: registered_users
	@title:
		Registered users
	@type:
		generic
	@short:
		Registration of users in KVIrc
	@keyterms:
		registered users, registration mask, registered user properties,
		user properties, notify property, avatar property
	@body:
		[big]Introduction[/big][br]
		The "registered user database" is basically a set of users with associated
		[doc:irc_masks]irc-masks[/doc] and properties.[br]
		It is used to recognize users on IRC and associate properties to them.[br]
		This works more or less like the IRC ban list, K-Line list, or invite list.[br]
		[big]User entry[/big][br]
		A registered user database entry is identified by an [b]unique[/b] name.[br]
		It may be the nickname of the user that you want to match, or the real name (if you know it)
		or any other string (even with spaces). The name is an "internal identifier" for the user entry:
		each name maps to a single entry and each entry has a single name.[br]
		Each entry has a set of registration [doc:irc_masks]irc-masks[/doc]: these masks
		are used to recognize the user on irc.[br]
		[br]
		[big]Registration masks[/big][br]
		The masks have the common IRC mask format: [b]<nick>!<user>@<host>[/b][br]
		The masks may contain '*' and '?' wildcards that match any portion of text.[br]
		[b]*!*@*[/b][br]
		[b]Pragma!*@*[/b][br]
		[b]*!~daemon@*[/b][br]
		[b]Pragma!*daemon@*.it[/b][br]
		[b]Pragma!?daemon@some*.it[/b][br]
		[b]Pragma!~daemon@some.host.it[/b][br]
		Are examples of valid registration masks.[br]
		The masks with wildcards can actually match more than a single user.[br]
		For example the mask *!root@*.host.com will match all the users
		having root as username and coming from the host.com domain.[br]
		For this reason putting wildcards in nicknames could become a problem
		if not used carefully (but may also be used to achieve interesting tricks).[br]
		If you don't use wildcards in nicknames you are sure that
		in a single irc connection , a mask will always refer to a single user.[br]
		You will commonly use the following format:[br]
		<nick>!*<username>@*.<host>.<top>[br]
		or[br]
		<nick>!*<username>@<number>.<number>.<number>.*[br]
		In this way you can be 95% sure that the mask will really match the correct user.[br]
		[br]
		[big]Example of registration and lookups[/big]
		Assume that you want to registere a friend of yours: Derek Riggs.[br]
		Derek often uses "Eddie" as his nickname
		"stranger" as username and has a dial-up connection that makes his IP address appear as
		<variable-number>.somewhere.in.time.org.[br]
		You will add an entry with name "Derek Riggs" and a registration mask like the following:
		Eddie!stranger@*.somewhere.in.time.org.[br]
		If the IRC servers keep adding strange characters ([doc:irc_masks]prefixes[/doc]) at the beginning of his username you may use
		Eddie!*stranger@*.somewhere.in.time.org.[br]
		If Eddie also often connects from the wasted.years.org domain and gets 'eddie' as username there, you might add a second registration mask as follows:
		Eddie!*eddie@*.wasted.years.org.[br]
		An alternative could be use only one mask with *.org as domain and allow any username (Eddie!*@*.org) but this
		could become dangerous since it could match the users that you don't want to.[br]
		On the other hand, if you dislike the users with the nickname Eddie that come from .org
		and you're implementing an auto-kick system, the correct mask to register is "Eddie!*@*.org".[br]
		[br]
		KVirc ties to be smart , and always find the most correct match for an user:
		If you have two masks registered: Pragma!*xor@*.myisp.it and *!*@*.myisp.it,
		kvirc will match Pragma!~xor@233-dyn.myisp.it with the first one even if the second
		one matches too; the firs one is a best match.[br]
		[br]
		[big]Properties[/big][br]
		A registered user has an (eventually empty) set of properties
		defined by name/value pairs. (In versions prior to 3.0.0 flags were used instead,
		but revealed to be insufficient).[br]
		KVirc recognizes some of these proprietes and associates semantic actions to it; other properties
		are left for scripting extension. Property names are case insensitive.[br]
		One of the recognized properties is the "[doc:notify_list]notify[/doc]" property.
		When an user is found to have this property set to a special value
		KVIrc will attempt to track the user presence on IRC.
		Another one is the [doc:avatar]avatar[/doc] property. Its value should be the
		name of the "default" [doc:avatar]avatar image file[/doc] for the specified user.[br]
		The "ignore" property should be set to "1" (or "true") for users that have to be ignored (:D).[br]
		[br]
		[big]The interface to the database[/big][br]
		The [module:reguser]reguser module[/module] is the interface to the "registered users database".[br]
		It provides a set of commands for adding and removing masks and manipulating properties.[br]
*/

//============================================================================================================
//
// KviRegisteredMask
//

KVILIB_API KviRegisteredUserDataBase* g_pRegisteredUserDataBase = 0;

KviRegisteredMask::KviRegisteredMask(KviRegisteredUser * u,KviIrcMask * m)
{
	m_pUser = u;
	m_pMask = m;
	m_iMaskNonWildChars = m_pMask->nonWildChars();
}

//============================================================================================================
//
// KviRegisteredUser
//


KviRegisteredUser::KviRegisteredUser(const QString & name)
{
	m_iIgnoreFlags  =0;
	m_bIgnoreEnabled=false;
	m_szName        = name;
	m_pPropertyDict = 0;
	m_pMaskList     = new QSet<KviIrcMask*>;
}

KviRegisteredUser::~KviRegisteredUser()
{
	foreach(KviIrcMask* m,*m_pMaskList)
	{
		delete m;
	}
	if(m_pPropertyDict)delete m_pPropertyDict;
	delete m_pMaskList;
}

bool KviRegisteredUser::isIgnoreEnabledFor(IgnoreFlags flag)
{
	if(!m_bIgnoreEnabled) return false;
	return m_iIgnoreFlags & flag;
}

KviIrcMask * KviRegisteredUser::findMask(const KviIrcMask &mask)
{
	foreach(KviIrcMask* m,*m_pMaskList)
	{
		if(*m == mask)return m;
	}
	return 0;
}

bool KviRegisteredUser::addMask(KviIrcMask * mask)
{
	if(findMask(*mask))
	{
		delete mask;
        mask = 0;
		return false;
	}
	m_pMaskList->insert(mask);
	return true;
}

bool KviRegisteredUser::removeMask(KviIrcMask * mask)
{
	if(!mask)return false;
	return m_pMaskList->remove(mask);
}

bool KviRegisteredUser::matches(const KviIrcMask &mask)
{
	foreach(KviIrcMask* m,*m_pMaskList)
	{
		if(m->matches(mask))return true;
	}
	return false;
}

bool KviRegisteredUser::matchesFixed(const KviIrcMask &mask)
{
	foreach(KviIrcMask* m,*m_pMaskList)
	{
		if(m->matchesFixed(mask))return true;
	}
	return false;
}

bool KviRegisteredUser::matchesFixed(const QString & nick,const QString & user,const QString & host)
{
	foreach(KviIrcMask* m,*m_pMaskList)
	{
		if(m->matchesFixed(nick,user,host))return true;
	}
	return false;
}

void KviRegisteredUser::setProperty(const QString &name,bool value)
{
	setProperty(name,value ? QString("true") : QString("false"));
}

void KviRegisteredUser::setProperty(const QString & name,const QString & value)
{
	if(!value.isEmpty())
	{
		if(!m_pPropertyDict)
		{
			m_pPropertyDict = new QHash<QString,QString>;
		}
		
		QString val = value.trimmed();

		if(!val.isEmpty())
		{
			m_pPropertyDict->insert(name,val);
		}
	} else {
		if(m_pPropertyDict)m_pPropertyDict->remove(name);
	}
}

bool KviRegisteredUser::getProperty(const QString & name,QString &value)
{
	if(!m_pPropertyDict)return false;
	if(name.isEmpty()) return false;
	if(m_pPropertyDict->contains(name))value = m_pPropertyDict->value(name);
	else return false;
	return true;
}

const QString & KviRegisteredUser::getProperty(const QString & name)
{
	if(!m_pPropertyDict)return KviQString::empty;
	if(name.isEmpty())return KviQString::empty;
	if(m_pPropertyDict->contains(name)) return m_pPropertyDict->value(name);
	return KviQString::empty;
}

bool KviRegisteredUser::getBoolProperty(const QString & name,bool def)
{
	if(!m_pPropertyDict)return def;
	if(name.isEmpty()) return def;
	if(m_pPropertyDict->contains(name))
	{
		KviStringConversion::fromString(m_pPropertyDict->value(name),def);
		return def;
	}
	return def;
}

//============================================================================================================
//
// KviRegisteredUserGroup
//

KviRegisteredUserGroup::KviRegisteredUserGroup(const QString &name)
{
	setName(name);
}

KviRegisteredUserGroup::~KviRegisteredUserGroup()
{
}


//============================================================================================================
//
// KviRegisteredUserDb
//

KviRegisteredUserDataBase::KviRegisteredUserDataBase()
{
	m_pUserDict = new QHash<QString,KviRegisteredUser*>; // do not copy keys

	m_pWildMaskList = new QSet<KviRegisteredMask*>;

	m_pMaskDict = new QHash<QString,KviRegisteredMaskList*>;
	
	m_pGroupDict = new  QHash<QString,KviRegisteredUserGroup*>;
}

KviRegisteredUserDataBase::~KviRegisteredUserDataBase()
{
	emit(databaseCleared());
	foreach(KviRegisteredUser* i,*m_pUserDict)
	{
		delete i;
	}
	foreach(KviRegisteredMask* i,*m_pWildMaskList)
	{
		delete i;
	}
	foreach(KviRegisteredMaskList* i,*m_pMaskDict)
	{
		delete i;
	}
	foreach(KviRegisteredUserGroup* i,*m_pGroupDict)
	{
		delete i;
	}
	delete m_pUserDict;
	delete m_pWildMaskList;
	delete m_pMaskDict;
	delete m_pGroupDict;
}

KviRegisteredUser * KviRegisteredUserDataBase::addUser(const QString & name)
{
	if(name.isEmpty()) return false;
	if(m_pUserDict->contains(name))return 0;
	KviRegisteredUser * u = new KviRegisteredUser(name);
	m_pUserDict->insert(u->name(),u); //u->name() because we're NOT copying keys!
	emit(userAdded(name));
	return u;
}

KviRegisteredUserGroup * KviRegisteredUserDataBase::addGroup(const QString & name)
{
	if(name.isEmpty()) return false;
	if(m_pGroupDict->contains(name))return 0;
	KviRegisteredUserGroup * pGroup = new KviRegisteredUserGroup(name);
	m_pGroupDict->insert(pGroup->name(),pGroup); //u->name() because we're NOT copying keys!
	return pGroup;
}

KviRegisteredUser * KviRegisteredUserDataBase::getUser(const QString & name)
{
	if(name.isEmpty()) return 0;
	KviRegisteredUser * u = m_pUserDict->value(name);
	if(!u)
	{
		u = new KviRegisteredUser(name);
		m_pUserDict->insert(u->name(),u); //u->name() because we're NOT copying keys!
	}
	return u;
}

//static void append_mask_to_list(KviRegisteredMaskList *l,KviRegisteredUser *u,KviIrcMask *mask)
//{
//	KviRegisteredMask * newMask = new KviRegisteredMask(u,mask);
//	foreach(KviRegisteredMask *m,*l)
//	{
//		if(m->nonWildChars() < newMask->nonWildChars())
//		{
//			l->remove()
//			l->insert(newMask);
//			return;
//		}
//	}
//	l->append(newMask);
//}

KviRegisteredUser * KviRegisteredUserDataBase::addMask(KviRegisteredUser * u,KviIrcMask * mask)
{
	if(!u || !mask) return 0;
	__range_valid(u == m_pUserDict->value(u->name()));

	if(!u->addMask(mask))
	{
		debug("ops...got an incoherent regusers action...recovered ?");
		return 0; // ops...already there ?
	}
	return u;
}

void KviRegisteredUserDataBase::copyFrom(KviRegisteredUserDataBase * db)
{
	m_pUserDict->clear();
	m_pWildMaskList->clear();
	m_pMaskDict->clear();
	m_pGroupDict->clear();
	emit(databaseCleared());

	QHash<QString,KviRegisteredUser*>::iterator it(db->m_pUserDict->begin());

	while(it!=db->m_pUserDict->end())
	{
		KviRegisteredUser * theCur = it.value();
		KviRegisteredUser * u = getUser(theCur->name());
		// copy masks
		QSet<KviIrcMask*> * l = theCur->maskList();
		foreach(KviIrcMask * m,*l)
		{
			KviIrcMask * m2 = new KviIrcMask(*m);
			addMask(u,m2);
		}
		// copy properties
		*(u->propertyDict())=*(theCur->propertyDict());
		
		u->m_iIgnoreFlags=theCur->m_iIgnoreFlags;
		u->m_bIgnoreEnabled=theCur->m_bIgnoreEnabled;
		u->setGroup(theCur->group());
		++it;
	}
	
	QHash<QString,KviRegisteredUserGroup*>::iterator git(db->m_pGroupDict->begin());
	while(git != db->m_pGroupDict->end())
	{
		addGroup(git.key());	
		++git;
	}
}


bool KviRegisteredUserDataBase::removeUser(const QString & name)
{
	if(name.isEmpty()) return false;
	KviRegisteredUser * u = m_pUserDict->value(name);
	if(!u)return false;
	foreach(KviIrcMask * mask,*(u->maskList()))
	{
		if(!removeMaskByPointer(mask))
			debug("Ops... removeMaskByPointer(%s) failed ?",KviQString::toUtf8(name).data());
	}
	emit(userRemoved(name));
	delete m_pUserDict->take(name);
	return true;
}
bool KviRegisteredUserDataBase::removeGroup(const QString & name)
{
	if(name.isEmpty()) return false;
	m_pGroupDict->remove(name);
	return true;
}

bool KviRegisteredUserDataBase::removeMask(const KviIrcMask &mask)
{
	// find the mask pointer
	KviRegisteredMask * m = findExactMask(mask);
	// and remove it
	if(m){
		if(removeMaskByPointer(m->mask()))
		{
			return true;
		}
	}
	return 0;
}

bool KviRegisteredUserDataBase::removeMaskByPointer(KviIrcMask * mask)
{
	if(!mask) return 0;
	if(mask->hasWildNick())
	{
		// remove from the wild list
		foreach(KviRegisteredMask * m,*m_pWildMaskList)
		{
			if(m->mask() == mask)
			{
				// ok..got it, remove from the list and from the user struct (user struct deletes it!)
				emit(userChanged(mask->nick()));
				m->user()->removeMask(mask);    // this one deletes m->mask()
				m_pWildMaskList->remove(m);  // this one deletes m
				return true;
			}
		}
		// not found ...opz :)
	} else {
		KviRegisteredMaskList * l = m_pMaskDict->value(mask->nick());
		if(l)
		{
			// FIXME: #warning "Here we could compare the host and username only: nick matches for sure"
			foreach(KviRegisteredMask * m,*l)
			{
				if(m->mask() == mask)
				{
					QString nick = mask->nick();
					emit(userChanged(nick));
					m->user()->removeMask(mask); // this one deletes m->mask() (or mask)
					l->remove(m);             // this one deletes m
					if(l->count() == 0)m_pMaskDict->remove(nick);
					return true;
				}
			}
			// not found ...opz
		}
	}
	// not found...
	return false;
}



/*
KviRegisteredUser * KviRegisteredUserDataBase::findMatchingUser(const KviIrcMask &mask)
{
	// first lookup the nickname in the maskDict
	KviRegisteredMaskList * l = m_pMaskDict->find(mask.nick());
	if(l)
	{
		for(KviRegisteredMask *m = l->first();m;m = l->next())
		{
			if(m->mask()->matchesFixed(0,mask.user(),mask.host()))return m->user();
		}
	}
	// not found....lookup the wild ones
	for(KviRegisteredMask * m = m_pWildMaskList->first();m;m = m_pWildMaskList->next())
	{
		if(m->mask()->matchesFixed(mask))return m->user();
	}
	return 0; // no match at all
}
*/
KviRegisteredUser * KviRegisteredUserDataBase::findMatchingUser(const QString & nick,const QString &user,const QString & host)
{
	KviRegisteredMask * m = findMatchingMask(nick,user,host);
	if(m)return m->user();
	return 0; // no match at all
}

KviRegisteredMask * KviRegisteredUserDataBase::findMatchingMask(const QString & nick,const QString &user,const QString & host)
{
	// first lookup the nickname in the maskDict
	if(nick.isEmpty()) return false;
	KviRegisteredMaskList * l = m_pMaskDict->value(nick);
	if(l)
	{
		foreach(KviRegisteredMask *m,*l)
		{
			if(m->mask()->matchesFixed(nick,user,host))return m;
		}
	}
	// not found....lookup the wild ones
	foreach(KviRegisteredMask *m,*m_pWildMaskList)
	{
		if(m->mask()->matchesFixed(nick,user,host))return m;
	}
	return 0; // no match at all
}

KviRegisteredUser * KviRegisteredUserDataBase::findUserWithMask(const KviIrcMask &mask)
{
	KviRegisteredMask * m = findExactMask(mask);
	if(m)return m->user();
	return 0;
}

KviRegisteredMask * KviRegisteredUserDataBase::findExactMask(const KviIrcMask &mask)
{
	// first lookup the nickname in the maskDict
	if(mask.nick()=="") return 0;
	KviRegisteredMaskList * l = m_pMaskDict->value(mask.nick());
	if(l)
	{
		foreach(KviRegisteredMask *m,*l)
		{
			if(*(m->mask()) == mask)return m;
		}
	}
	// not found....lookup the wild ones
	foreach(KviRegisteredMask *m,*m_pWildMaskList)
	{
		if(*(m->mask()) == mask)return m;
	}
	return 0; // no match at all
}
/*
bool KviRegisteredUserDataBase::isIgnoredUser(const QString & nick,const QString & user,const QString & host)
{ 
	KviRegisteredUser * u = findMatchingUser(nick,user,host);
	if(u)return u->getBoolProperty("IGNORE");
	else return false;
}
*/
void KviRegisteredUserDataBase::load(const QString & filename)
{
	QString szCurrent;
	KviConfig cfg(filename,KviConfig::Read);

	KviConfigIterator it(cfg.dict()->begin());
	while(it!=cfg.dict()->end())
	{
		cfg.setGroup(it.key());
		szCurrent=it.key();
		if(KviQString::equalCSN("#Group ",szCurrent,7))
		{
			szCurrent.remove(0,7);
			addGroup(szCurrent);
		} else {
			KviRegisteredUser * u = addUser(szCurrent);
			
			if(u)
			{
				u->setIgnoreEnabled(cfg.readBoolEntry("IgnoreEnabled",false));
				u->setIgnoreFlags(cfg.readIntEntry("IgnoreFlags",0));
				KviConfigGroup::iterator sdi(it.value()->begin());
				while(sdi != it.value()->end())
				{
					QString tmp = sdi.key();
					if(KviQString::equalCSN("prop_",tmp,5))
					{
						tmp.remove(0,5);
						u->setProperty(tmp,sdi.value());
					} else if(KviQString::equalCSN("mask_",tmp,5))
					{
						KviIrcMask * mask = new KviIrcMask(sdi.value());
						addMask(u,mask);
					} else if(KviQString::equalCI(tmp,"Group"))
					{
						u->setGroup(sdi.value());
					}
					++sdi;
				}
			}
		}
		++it;
	}
	if(!m_pGroupDict->value(__tr("Default")))
		addGroup(__tr("Default"));
}


void KviRegisteredUserDataBase::save(const QString & filename)
{
	KviConfig cfg(filename,KviConfig::Write);
	cfg.clear();
	cfg.preserveEmptyGroups(true);

	foreach(KviRegisteredUser *u, *m_pUserDict)
	{
		cfg.setGroup(u->name());
		// Write properties
		cfg.writeEntry("IgnoreEnabled",u->ignoreEnagled());
		cfg.writeEntry("IgnoreFlags",u->ignoreFlags());
		if(u->propertyDict())
		{
			QHash<QString,QString>::iterator pit(u->propertyDict()->begin());
			while(pit != u->propertyDict()->end())
			{
				QString tmp = "prop_";
				tmp.append(pit.key());
				cfg.writeEntry(tmp,pit.value());
				++pit;
			}
		}
		// Write masks
		int idx = 0;
		foreach(KviIrcMask * m,*(u->maskList()))
		{
			QString tmp;
			KviQString::sprintf(tmp,"mask_%d",idx);
			QString mask;
			m->mask(mask,KviIrcMask::NickUserHost);
			cfg.writeEntry(tmp,mask);
			++idx;
		}
		cfg.writeEntry("Group",u->group());
	}
	
	QString szTmp;
	foreach(KviRegisteredUserGroup*u,*m_pGroupDict)
	{
		KviQString::sprintf(szTmp,"#Group %Q",&(u->name()));
		cfg.setGroup(szTmp);
	}
	
}
