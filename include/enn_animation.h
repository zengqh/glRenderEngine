/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/05
* File: enn_animation.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_math.h"

namespace enn
{
class AniTrack;

//////////////////////////////////////////////////////////////////////////
// KeyFrame
class KeyFrame : public AllocatedObject
{
public:
	KeyFrame(AniTrack* owner, float time) : owner_(owner), time_(time)
	{

	}
	
	virtual ~KeyFrame()
	{

	}

	float getTime() const
	{
		return time_;
	}

protected:
	AniTrack*		owner_;
	float			time_;
};

//////////////////////////////////////////////////////////////////////////
// NumKeyFrame
class NumKeyFrame : public KeyFrame
{
public:
	NumKeyFrame(AniTrack* owner, float time)
		: KeyFrame(owner, time)
	{

	}

	~NumKeyFrame()
	{

	}

public:
	void setNum(float num)
	{
		num_ = num;
	}

	float getNum() const
	{
		return num_;
	}

protected:
	float num_;
};

//////////////////////////////////////////////////////////////////////////
// NodeKeyFrame
class NodeKeyFrame : public KeyFrame
{
public:
	NodeKeyFrame(AniTrack* owner, float time)
		: KeyFrame(owner, time)
	{

	}

	~NodeKeyFrame()
	{

	}

public:
	void setPosition(const vec3f& pos)
	{
		pos_ = pos;
	}

	void setRotation(const Quaternion& rot)
	{
		rot_ = rot;
	}

	void setScale(const vec3f& scale)
	{
		scale_ = scale;
	}

public:
	const vec3f& getPosition() const
	{
		return pos_;
	}

	const Quaternion& getRotation() const
	{
		return rot_;
	}

	const vec3f& getScale() const
	{
		return scale_;
	}

protected:
	vec3f				pos_;
	Quaternion			rot_;
	vec3f				scale_;
};

class Animation;

//////////////////////////////////////////////////////////////////////////
// AniTimeIndex
class AniTimeIndex
{
public:
	AniTimeIndex( float timePos ) : time_pos_(timePos) {}

public:
	float getTimePos() const
	{
		return time_pos_;
	}

private:
	float time_pos_;
};

//////////////////////////////////////////////////////////////////////////
// AniKeyFrameTimeLess
class AniKeyFrameTimeLess
{
public:
	bool operator()( const KeyFrame* kf1, const KeyFrame* kf2 ) const
	{
		return kf1->getTime() < kf2->getTime();
	}
};

//////////////////////////////////////////////////////////////////////////
// AniTrack
class IAniCtrlObjSet;
class AniTrack : public AllocatedObject
{
public:
	typedef enn::vector<KeyFrame*>::type KeyFrameList;

public:
	AniTrack(Animation* owner, int id);
	virtual ~AniTrack();

public:
	KeyFrame* createKeyFrame(float time_pos);
	void removeKeyFrame(int idx);
	void removeAllKeyFrames();

public:
	int getKeyFrameCount() const;
	KeyFrame* getKeyFrame(int idx) const;

public:
	void setLoop(bool b);
	bool isLoop() const;

public:
	virtual void apply( IAniCtrlObjSet* ctrlObjSet, const AniTimeIndex& timeIndex ) = 0;

	virtual void getInterpKeyFrame( const AniTimeIndex& time_idx, KeyFrame* key_frame ) = 0;

	virtual KeyFrame* createKeyFrameImpl( float timePos ) = 0;

protected:
	float getKeyFramesAtTime( const AniTimeIndex& timeIndex, KeyFrame** keyFrame1,
		KeyFrame** keyFrame2, int* firstKeyIndex ) const;

protected:
	Animation*		owner_;
	int				id_;
	KeyFrameList	key_frames_;
	bool			is_loop_;
};

//////////////////////////////////////////////////////////////////////////
// NumAniTrack
class NumAniTrack : public AniTrack
{
public:
	NumAniTrack(Animation* owner, int id);
	~NumAniTrack();

public:
	virtual void apply( IAniCtrlObjSet* ctrlObjSet, const AniTimeIndex& timeIndex );

	virtual void getInterpKeyFrame(const AniTimeIndex& time_idx, KeyFrame* key_frame);

	virtual KeyFrame* createKeyFrameImpl( float timePos );
};

//////////////////////////////////////////////////////////////////////////
// NodeAniTrack
class NodeAniTrack : public AniTrack
{
public:
	NodeAniTrack(Animation* owner, int id);
	~NodeAniTrack();

public:
	virtual void apply( IAniCtrlObjSet* ctrlObjSet, const AniTimeIndex& timeIndex );

	virtual void getInterpKeyFrame(const AniTimeIndex& time_idx, KeyFrame* key_frame);
	
	virtual KeyFrame* createKeyFrameImpl( float timePos );
};

//////////////////////////////////////////////////////////////////////////
// IAniCtrlObjSet
class IAniCtrlObjSet
{
public:
	virtual ~IAniCtrlObjSet() {}

public:
	virtual void setNodeValue( int trackId, NodeKeyFrame* kf ) {}

	virtual void setNumValue( int trackId, NumKeyFrame* kf ) {}
};

//////////////////////////////////////////////////////////////////////////
// Animation
class Animation : public AllocatedObject
{
public:
	typedef enn::map<int, NodeAniTrack*>::type NodeAniTrackList;
	typedef enn::map<int, NumAniTrack*>::type  NumAniTrackList;

public:
	Animation(int id);
	~Animation();

public:
	int getId() const;
	void clear();

	void setLength(float len);
	void setAutoLength();
	float getLength() const;

	NodeAniTrack* createNodeTrack(int id);
	NumAniTrack* createNumTrack(int id);

	int getNumNodeTracks() const;
	int getNumNumTracks() const;

	NodeAniTrack* getNodeTrack(int id) const;
	NumAniTrack* getNumTrack(int id) const;

	void destroyNodeTrack(int id);
	void destroyNumTrack(int id);

	void destroyAllNodeTracks();
	void destroyAllNumTracks();

public:
	void apply( IAniCtrlObjSet* ctrlObjSet, float timePos );

protected:
	int						id_;
	float					length_;
	NodeAniTrackList		node_track_list_;
	NumAniTrackList			num_track_list_;
};

//////////////////////////////////////////////////////////////////////////
class AnimationState;
struct AniTimeTrigger
{
	AniTimeTrigger() : timePos(0), userData(0) {}

	AniTimeTrigger( float t, int u ) : timePos(t), userData(u) {}

	float   timePos;
	int     userData;
};

struct IAniStateEvent
{
	virtual void onAniTimeStart( AnimationState* aniState ) = 0;

	virtual void onAniTimeEnd( AnimationState* aniState ) = 0;

	virtual void onAniTimeTrigger( AnimationState* aniState, const AniTimeTrigger* trigger ) = 0;
};
//////////////////////////////////////////////////////////////////////////
// AnimationState
class AnimationState : public AllocatedObject
{
	typedef std::vector<AniTimeTrigger> TimeTriggers;
public:
	AnimationState();
	~AnimationState();

public:
	void setAnimation(Animation* ani);
	Animation* getAnimation() const;

	void setAniCtrlObjSet(IAniCtrlObjSet* obj);
	IAniCtrlObjSet* getCtrlObjSet() const;

	void setLoop(bool v);
	bool isLoop() const;

	void setEnable(bool v);
	bool isEnable() const;

	void setSpeed(float v);
	float getSpeed() const;

	void setWeight(float v);
	float getWeight() const;

	void setTimePos(float time);
	void addTime(float time);

public:
	void setEvent(IAniStateEvent* evt);
	void addTimeTrigger(float timePos, int userData);
	void clearAllTimeTriggers();

protected:
	Animation*				ani_;
	IAniCtrlObjSet*			ani_ctrl_obj_set_;
	IAniStateEvent*			evt_;
	TimeTriggers			time_triggers_;
	float					weight_;
	float					speed_;
	float					time_pos_;

	bool					is_loop_;
	bool					is_enable_;
};
}