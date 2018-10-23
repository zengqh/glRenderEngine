/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/05
* File: enn_animation.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_animation.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
// AniTrack
AniTrack::AniTrack(Animation* owner, int id)
: owner_(owner)
, id_(id)
{

}

AniTrack::~AniTrack()
{
	removeAllKeyFrames();
}

KeyFrame* AniTrack::createKeyFrame(float time_pos)
{
	KeyFrame* kf = createKeyFrameImpl(time_pos);

	KeyFrameList::iterator iter = std::upper_bound( key_frames_.begin(), 
		key_frames_.end(), kf, AniKeyFrameTimeLess());

	key_frames_.insert(iter, kf);

	return kf;
}

void AniTrack::removeKeyFrame(int idx)
{
	if ( 0 <= idx && idx < (int)key_frames_.size() )
	{
		KeyFrame* kf = key_frames_[idx];
		ENN_SAFE_DELETE(kf);
		key_frames_.erase( key_frames_.begin() + idx );
	}
}

void AniTrack::removeAllKeyFrames()
{
	while ( !key_frames_.empty() )
	{
		removeKeyFrame(0);
	}
}

int AniTrack::getKeyFrameCount() const
{
	return (int)key_frames_.size();
}

KeyFrame* AniTrack::getKeyFrame(int idx) const
{
	if ( 0 <= idx && idx < (int)key_frames_.size() )
	{
		return key_frames_[idx];
	}

	return 0;
}

void AniTrack::setLoop(bool b)
{
	is_loop_ = b;
}

bool AniTrack::isLoop() const
{
	return is_loop_;
}

float AniTrack::getKeyFramesAtTime( const AniTimeIndex& timeIndex, KeyFrame** keyFrame1,
						 KeyFrame** keyFrame2, int* firstKeyIndex ) const
{
	ENN_ASSERT( !key_frames_.empty() );

	float cur_time_pos = timeIndex.getTimePos();
	float total_time_len = owner_->getLength();

	if ( total_time_len > 0 )
	{
		while ( cur_time_pos > total_time_len )
		{
			cur_time_pos -= total_time_len;
		}
	}

	if (isLoop())
	{
		float first_time = key_frames_.front()->getTime();
		float last_time = key_frames_.back()->getTime();

		float len = last_time - first_time;

		if (len > 0)
		{
			while (cur_time_pos > last_time)
			{
				cur_time_pos -= len;
			}
		}
	}

	KeyFrame tmp_key(0, cur_time_pos);

	KeyFrameList::const_iterator iter =
		std::lower_bound(key_frames_.begin(), key_frames_.end(), 
		&tmp_key, AniKeyFrameTimeLess());

	/** can not find */
	if (iter == key_frames_.end())
	{
		*keyFrame1 = key_frames_.back();
		*keyFrame2 = *keyFrame1;
		*firstKeyIndex = (int)key_frames_.size() - 1;

		return 0.0f;
	}

	*keyFrame2 = *iter;

	/** if the second key frame is at the begin !!! */
	if (iter == key_frames_.begin())
	{
		*keyFrame1 = *keyFrame2;
		*firstKeyIndex = 0;
		return 0.0f;
	}

	/** normal condition */
	--iter;
	*keyFrame1 = *iter;
	*firstKeyIndex = std::distance( key_frames_.begin(), iter );
	float t = (cur_time_pos - (*keyFrame1)->getTime()) / ((*keyFrame2)->getTime() - (*keyFrame1)->getTime());

	return t;
}

//////////////////////////////////////////////////////////////////////////
// NodeAniTrack
NodeAniTrack::NodeAniTrack(Animation* owner, int id)
: AniTrack(owner, id)
{

}

NodeAniTrack::~NodeAniTrack()
{

}

void NodeAniTrack::apply( IAniCtrlObjSet* ctrlObjSet, const AniTimeIndex& timeIndex )
{
	NodeKeyFrame kf(0, timeIndex.getTimePos());
	getInterpKeyFrame( timeIndex, &kf );

	ctrlObjSet->setNodeValue( id_, &kf );
}

void NodeAniTrack::getInterpKeyFrame(const AniTimeIndex& time_idx, KeyFrame* key_frame)
{
	if (key_frames_.empty())
	{
		NodeKeyFrame* nkf = static_cast<NodeKeyFrame*>(key_frame);
		nkf->setPosition( vec3f() );
		nkf->setRotation( Quaternion() );
		nkf->setScale( vec3f(1.0f, 1.0f, 1.0f) );

		return;
	}

	KeyFrame* k1    = 0;
	KeyFrame* k2    = 0;
	int k1Idx = 0;

	float t = getKeyFramesAtTime( time_idx, &k1, &k2, &k1Idx );

	NodeKeyFrame* nk1 = static_cast<NodeKeyFrame*>(k1);
	NodeKeyFrame* nk2 = static_cast<NodeKeyFrame*>(k2);
	NodeKeyFrame* nkf = static_cast<NodeKeyFrame*>(key_frame);

	/** 在第1关键帧头 */
	if (t == 0)
	{
		nkf->setPosition( nk1->getPosition() );
		nkf->setRotation( nk1->getRotation() );
		nkf->setScale( nk1->getScale() );
	}
	/** 在关键帧1,2之间，那么插值 */
	else
	{
		/** pos之间插值 */
		vec3f newPos = nk1->getPosition() + (nk2->getPosition() - nk1->getPosition()) * t;
		nkf->setPosition( newPos );

		/** 方位之间四元素插值 */
		Quaternion newDir = Quaternion::slerp(t, nk1->getRotation(), nk2->getRotation(), true);
		nkf->setRotation( newDir );

		/** scale之间插值 */
		vec3f newScale = nk1->getScale() + (nk2->getScale() - nk1->getScale()) * t;
		nkf->setScale( newScale );
	}
}

KeyFrame* NodeAniTrack::createKeyFrameImpl( float timePos )
{
	return ENN_NEW NodeKeyFrame(this, timePos);
}

//////////////////////////////////////////////////////////////////////////
// NumAniTrack
NumAniTrack::NumAniTrack(Animation* owner, int id)
: AniTrack(owner, id)
{

}

NumAniTrack::~NumAniTrack()
{

}

void NumAniTrack::apply( IAniCtrlObjSet* ctrlObjSet, const AniTimeIndex& timeIndex )
{
	NumKeyFrame kf(0, timeIndex.getTimePos());
	getInterpKeyFrame( timeIndex, &kf );
	ctrlObjSet->setNumValue( id_, &kf );
}

void NumAniTrack::getInterpKeyFrame(const AniTimeIndex& time_idx, KeyFrame* key_frame)
{
	if ( key_frames_.empty() )
	{
		NumKeyFrame* nkf = static_cast<NumKeyFrame*>(key_frame);
		nkf->setNum( 0.0f );
		return;
	}

	KeyFrame* k1    = 0;
	KeyFrame* k2    = 0;
	int k1Idx = 0;

	float t = getKeyFramesAtTime( time_idx, &k1, &k2, &k1Idx );

	NumKeyFrame* nk1 = static_cast<NumKeyFrame*>(k1);
	NumKeyFrame* nk2 = static_cast<NumKeyFrame*>(k2);
	NumKeyFrame* nkf = static_cast<NumKeyFrame*>(key_frame);

	/** 在第1关键帧头 */
	if ( t == 0 ) 
	{
		nkf->setNum( nk1->getNum() );
	}
	/** 在关键帧1,2之间，那么插值 */
	else 
	{
		/** pos之间插值 */
		float num = nk1->getNum() + (nk2->getNum() - nk1->getNum()) * t;
		nkf->setNum( num );
	}
}

KeyFrame* NumAniTrack::createKeyFrameImpl( float timePos )
{
	return ENN_NEW NumKeyFrame(this, timePos);
}

//////////////////////////////////////////////////////////////////////////
// Animation
Animation::Animation(int id)
: id_(id)
{

}

Animation::~Animation()
{
	clear();
}

int Animation::getId() const
{
	return id_;
}

void Animation::clear()
{
	setLength(0.0f);
	destroyAllNodeTracks();
	destroyAllNumTracks();
}

void Animation::setLength(float len)
{
	if (len < 0)
	{
		len = 0;
	}

	length_ = len;
}

/** 根据最长的关键帧时间点决定动画长度 */
void Animation::setAutoLength()
{
	float len = 0;

	for ( NodeAniTrackList::const_iterator it = node_track_list_.begin(), ite = node_track_list_.end(); it != ite; ++it )
	{
		const NodeAniTrack* track = it->second;

		int kcnt = track->getKeyFrameCount();

		if ( kcnt > 0 )
		{
			float t = track->getKeyFrame(kcnt - 1)->getTime();

			if ( t > len )
			{
				len = t;
			}
		}
	}

	for ( NumAniTrackList::const_iterator it = num_track_list_.begin(), ite = num_track_list_.end(); it != ite; ++it )
	{
		const NumAniTrack* track = it->second;

		int kcnt = track->getKeyFrameCount();

		if ( kcnt > 0 )
		{
			float t = track->getKeyFrame(kcnt - 1)->getTime();

			if ( t > len )
			{
				len = t;
			}
		}
	}

	length_ = len;
}

float Animation::getLength() const
{
	return length_;
}

NodeAniTrack* Animation::createNodeTrack(int id)
{
	if (getNodeTrack(id))
	{
		return 0;
	}

	NodeAniTrack* track = ENN_NEW NodeAniTrack(this, id);
	node_track_list_[id] = track;

	return track;
}

NumAniTrack* Animation::createNumTrack(int id)
{
	if (getNumTrack(id))
	{
		return 0;
	}

	NumAniTrack* track = ENN_NEW NumAniTrack(this, id);
	num_track_list_[id] = track;

	return track;
}

int Animation::getNumNodeTracks() const
{
	return (int)(node_track_list_.size());
}

int Animation::getNumNumTracks() const
{
	return (int)(num_track_list_.size());
}

NodeAniTrack* Animation::getNodeTrack(int id) const
{
	NodeAniTrackList::const_iterator iter = node_track_list_.find(id);
	if (iter != node_track_list_.end())
	{
		return iter->second;
	}

	return 0;
}

NumAniTrack* Animation::getNumTrack(int id) const
{
	NumAniTrackList::const_iterator iter = num_track_list_.find(id);
	if (iter != num_track_list_.end())
	{
		return iter->second;
	}

	return 0;
}

void Animation::destroyNodeTrack(int id)
{
	NodeAniTrackList::iterator iter = node_track_list_.find(id);
	if (iter != node_track_list_.end())
	{
		NodeAniTrack* track = iter->second;
		ENN_SAFE_DELETE(track);
		node_track_list_.erase(iter);
	}
}

void Animation::destroyNumTrack(int id)
{
	NumAniTrackList::iterator iter = num_track_list_.find(id);
	if (iter != num_track_list_.end())
	{
		NumAniTrack* track = iter->second;
		ENN_SAFE_DELETE(track);
		num_track_list_.erase(iter);
	}
}

void Animation::destroyAllNodeTracks()
{
	NodeAniTrackList::iterator iter = node_track_list_.begin();
	for (; iter != node_track_list_.end(); ++iter)
	{
		NodeAniTrack* track = iter->second;
		ENN_SAFE_DELETE(track);
	}

	node_track_list_.clear();
}

void Animation::destroyAllNumTracks()
{
	NumAniTrackList::iterator iter = num_track_list_.begin();
	for (; iter != num_track_list_.end(); ++iter)
	{
		NumAniTrack* track = iter->second;
		ENN_SAFE_DELETE(track);
	}

	num_track_list_.clear();
}

void Animation::apply( IAniCtrlObjSet* ctrlObjSet, float timePos )
{
	AniTimeIndex timeIdx(timePos);

	for ( NumAniTrackList::iterator it = num_track_list_.begin(), ite = num_track_list_.end(); it != ite; ++it )
	{
		NumAniTrack* track = it->second;
		track->apply( ctrlObjSet, timeIdx );
	}

	for ( NodeAniTrackList::iterator it = node_track_list_.begin(), ite = node_track_list_.end(); it != ite; ++it )
	{
		NodeAniTrack* track = it->second;
		track->apply( ctrlObjSet, timeIdx );
	}
}

//////////////////////////////////////////////////////////////////////////
struct TriggerCmpByTime
{
	bool operator()( const AniTimeTrigger& lhs, const AniTimeTrigger& rhs ) const
	{
		return lhs.timePos < rhs.timePos;
	}
};

//////////////////////////////////////////////////////////////////////////
// AnimationState
AnimationState::AnimationState()
: ani_(0)
, ani_ctrl_obj_set_(0)
, evt_(0)
, weight_(1.0f)
, speed_(1.0f)
, is_loop_(false)
, is_enable_(false)
, time_pos_(0)
{

}

AnimationState::~AnimationState()
{

}

void AnimationState::setAnimation(Animation* ani)
{
	ani_ = ani;
}

Animation* AnimationState::getAnimation() const
{
	return ani_;
}

void AnimationState::setAniCtrlObjSet(IAniCtrlObjSet* obj)
{
	ani_ctrl_obj_set_ = obj;
}

IAniCtrlObjSet* AnimationState::getCtrlObjSet() const
{
	return ani_ctrl_obj_set_;
}

void AnimationState::setLoop(bool v)
{
	is_loop_ = v;
}

bool AnimationState::isLoop() const
{
	return is_loop_;
}

void AnimationState::setEnable(bool v)
{
	is_enable_ = v;
}

bool AnimationState::isEnable() const
{
	return is_enable_;
}

void AnimationState::setSpeed(float v)
{
	speed_ = v;
}

float AnimationState::getSpeed() const
{
	return speed_;
}

void AnimationState::setWeight(float v)
{
	weight_ = v;
}

float AnimationState::getWeight() const
{
	return weight_;
}

void AnimationState::setTimePos(float time)
{
	float total_len = ani_->getLength();

	if (total_len > 0)
	{
		if (isLoop())
		{
			while (time > total_len)
			{
				time -= total_len;
			}
		}
		else
		{
			if (time >= total_len)
			{
				time_pos_ = total_len;
				if (isEnable())
				{
					ani_->apply(ani_ctrl_obj_set_, time_pos_);
				}

				setEnable(false);
				return;
			}
		}

		time_pos_ = time;
		if (isEnable())
		{
			ani_->apply(ani_ctrl_obj_set_, time_pos_);
		}
	}
}

void AnimationState::addTime(float time)
{
	setTimePos(time_pos_ + speed_ * time);
}

void AnimationState::setEvent(IAniStateEvent* evt)
{
	evt_ = evt;
}

void AnimationState::addTimeTrigger(float timePos, int userData)
{
	time_triggers_.push_back(AniTimeTrigger(timePos, userData));

	std::sort(time_triggers_.begin(), time_triggers_.end(), TriggerCmpByTime());
}

void AnimationState::clearAllTimeTriggers()
{
	time_triggers_.clear();
}

}