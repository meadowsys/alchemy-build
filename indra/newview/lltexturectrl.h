/** 
 * @file lltexturectrl.h
 * @author Richard Nelson, James Cook
 * @brief LLTextureCtrl class header file including related functions
 *
 * $LicenseInfo:firstyear=2002&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */

#ifndef LL_LLTEXTURECTRL_H
#define LL_LLTEXTURECTRL_H

#include "llfloater.h"
#include "llfolderview.h"
#include "lllocalbitmaps.h"
#include "lluictrl.h"
#include "llpermissionsflags.h"
#include "lltextbox.h" // for params
#include "llviewerinventory.h"
#include "llviewborder.h" // for params
#include "llviewerobject.h"
#include "llviewertexture.h"
#include "llwindow.h"

class LLComboBox;
class LLFloaterTexturePicker;
class LLInventoryItem;
class LLViewerFetchedTexture;
class LLFetchedGLTFMaterial;

// used for setting drag & drop callbacks.
typedef boost::function<BOOL (LLUICtrl*, LLInventoryItem*)> drag_n_drop_callback;
typedef boost::function<void (LLInventoryItem*)> texture_selected_callback;

// Helper functions for UI that work with picker
bool get_is_predefined_texture(LLUUID asset_id);

// texture picker works by asset ids since objects normaly do
// not retain inventory ids as result these functions are looking
// for textures in inventory by asset ids
// This search can be performance unfriendly and doesn't warranty
// that the texture is original source of asset
LLUUID get_copy_free_item_by_asset_id(LLUUID image_id, bool no_trans_perm = false);
bool get_can_copy_texture(LLUUID image_id);

//////////////////////////////////////////////////////////////////////////////////////////
// LLTextureCtrl


class LLTextureCtrl
: public LLUICtrl
{
public:
	typedef enum e_texture_pick_op
	{
		TEXTURE_CHANGE,
		TEXTURE_SELECT,
		TEXTURE_CANCEL
	} ETexturePickOp;

    typedef enum e_pick_inventory_type
    {
        PICK_TEXTURE_MATERIAL = 0,
        PICK_TEXTURE = 1,
        PICK_MATERIAL = 2,
    } EPickInventoryType;

public:
	struct Params : public LLInitParam::Block<Params, LLUICtrl::Params>
	{
		Optional<LLUUID>		image_id;
		Optional<LLUUID>		default_image_id;
		Optional<std::string>	default_image_name;
		Optional<bool>			allow_no_texture;
		Optional<bool>			can_apply_immediately;
		Optional<bool>			no_commit_on_selection; // alternative mode: commit occurs and the widget gets dirty
														// only on DnD or when OK is pressed in the picker
		Optional<S32>			label_width;
		Optional<LLUIColor>		border_color;
		Optional<LLUIImage*>	fallback_image;
		
		Optional<LLTextBox::Params>	multiselect_text,
									caption_text;

		Optional<LLViewBorder::Params> border;

		Params()
		:	image_id("image"),
			default_image_id("default_image_id"),
			default_image_name("default_image_name"),
			allow_no_texture("allow_no_texture", false),
			can_apply_immediately("can_apply_immediately"),
			no_commit_on_selection("no_commit_on_selection", false),
		    label_width("label_width", -1),
			border_color("border_color"),
			fallback_image("fallback_image"),
			multiselect_text("multiselect_text"),
			caption_text("caption_text"),
			border("border")
		{}
	};
protected:
	LLTextureCtrl(const Params&);
	friend class LLUICtrlFactory;
public:
	virtual ~LLTextureCtrl();

	// LLView interface

	virtual BOOL	handleMouseDown(S32 x, S32 y, MASK mask);
	virtual BOOL	handleDragAndDrop(S32 x, S32 y, MASK mask,
						BOOL drop, EDragAndDropType cargo_type, void *cargo_data,
						EAcceptance *accept,
						std::string& tooltip_msg);
	virtual BOOL	handleHover(S32 x, S32 y, MASK mask);
	virtual BOOL	handleUnicodeCharHere(llwchar uni_char);

	virtual void	draw();
	virtual void	setVisible( BOOL visible );
	virtual void	setEnabled( BOOL enabled );

	void			setValid(BOOL valid);

	// LLUICtrl interface
	virtual void	clear();

	// Takes a UUID, wraps get/setImageAssetID
	virtual void	setValue(const LLSD& value);
	virtual LLSD	getValue() const;

	// LLTextureCtrl interface
	void			showPicker(BOOL take_focus);
	bool			isPickerShown() { return !mFloaterHandle.isDead(); }
	void			setLabel(const std::string& label);
	void			setLabelWidth(S32 label_width) {mLabelWidth =label_width;}	
	const std::string&	getLabel() const							{ return mLabel; }

	void			setAllowNoTexture( BOOL b )					{ mAllowNoTexture = b; }
	bool			getAllowNoTexture() const					{ return mAllowNoTexture; }

	void			setAllowLocalTexture(BOOL b)					{ mAllowLocalTexture = b; }
	BOOL			getAllowLocalTexture() const					{ return mAllowLocalTexture; }

	const LLUUID&	getImageItemID() { return mImageItemID; }

	virtual void	setImageAssetName(const std::string& name);
	
	void			setImageAssetID(const LLUUID &image_asset_id);
	const LLUUID&	getImageAssetID() const						{ return mImageAssetID; }

	void			setDefaultImageAssetID( const LLUUID& id )	{ mDefaultImageAssetID = id; }
	const LLUUID&	getDefaultImageAssetID() const { return mDefaultImageAssetID; }

	const std::string&	getDefaultImageName() const					{ return mDefaultImageName; }

	void			setBlankImageAssetID( const LLUUID& id )	{ mBlankImageAssetID = id; }
	const LLUUID&	getBlankImageAssetID() const { return mBlankImageAssetID; }

	void			setOpenTexPreview(bool open_preview) { mOpenTexPreview = open_preview; }

	void			setTransparentImageAssetID( const LLUUID& id )	{ mTransparentImageAssetID = id; }
	const LLUUID&	getTransparentImageAssetID() const { return mTransparentImageAssetID; }

	void			setCaption(const std::string& caption);
	void			setCanApplyImmediately(BOOL b);

	void			setCanApply(bool can_preview, bool can_apply);

	void			setImmediateFilterPermMask(PermissionMask mask);
	void			setDnDFilterPermMask(PermissionMask mask)
						{ mDnDFilterPermMask = mask; }
	PermissionMask	getImmediateFilterPermMask() { return mImmediateFilterPermMask; }

	void			closeDependentFloater();

	void			onFloaterClose();
	void			onFloaterCommit(ETexturePickOp op, LLUUID id);

	// This call is returned when a drag is detected. Your callback
	// should return TRUE if the drag is acceptable.
	void setDragCallback(drag_n_drop_callback cb)	{ mDragCallback = cb; }

	// This callback is called when the drop happens. Return TRUE if
	// the drop happened - resulting in an on commit callback, but not
	// necessariliy any other change.
	void setDropCallback(drag_n_drop_callback cb)	{ mDropCallback = cb; }
	
	void setOnCancelCallback(commit_callback_t cb)	{ mOnCancelCallback = cb; }
	void setOnCloseCallback(commit_callback_t cb)	{ mOnCloseCallback = cb; }
	void setOnSelectCallback(commit_callback_t cb)	{ mOnSelectCallback = cb; }

	/*
	 * callback for changing texture selection in inventory list of texture floater
	 */
	void setOnTextureSelectedCallback(texture_selected_callback cb);

	void setShowLoadingPlaceholder(BOOL showLoadingPlaceholder);

	LLViewerFetchedTexture* getTexture() { return mTexturep; }

    void setBakeTextureEnabled(bool enabled);
    bool getBakeTextureEnabled() const { return mBakeTextureEnabled; }

    void setInventoryPickType(EPickInventoryType type);
    EPickInventoryType getInventoryPickType() { return mInventoryPickType; };

private:
	BOOL allowDrop(LLInventoryItem* item, EDragAndDropType cargo_type, std::string& tooltip_msg);
	BOOL doDrop(LLInventoryItem* item);

private:
	drag_n_drop_callback	 	mDragCallback;
	drag_n_drop_callback	 	mDropCallback;
	commit_callback_t		 	mOnCancelCallback;
	commit_callback_t		 	mOnSelectCallback;
	commit_callback_t		 	mOnCloseCallback;
	texture_selected_callback	mOnTextureSelectedCallback;
	LLPointer<LLViewerFetchedTexture> mTexturep;
	LLUIColor				 	mBorderColor;
	LLUUID					 	mImageItemID;
	LLUUID					 	mImageAssetID;
	LLUUID					 	mDefaultImageAssetID;
	LLUUID					 	mBlankImageAssetID;
	LLUUID					 	mTransparentImageAssetID;
	LLUIImagePtr				mFallbackImage;
	std::string					mDefaultImageName;
	LLHandle<LLFloater>			mFloaterHandle;
	LLTextBox*				 	mTentativeLabel;
	LLTextBox*				 	mCaption;
	std::string				 	mLabel;
	BOOL					 	mAllowNoTexture; // If true, the user can select "none" as an option
	BOOL						mAllowLocalTexture;
	PermissionMask			 	mImmediateFilterPermMask;
	PermissionMask				mDnDFilterPermMask;
	BOOL					 	mCanApplyImmediately;
	BOOL					 	mCommitOnSelection;
	BOOL					 	mNeedsRawImageData;
	LLViewBorder*			 	mBorder;
	BOOL					 	mValid;
	BOOL					 	mShowLoadingPlaceholder;
	std::string				 	mLoadingPlaceholderString;
	S32						 	mLabelWidth;
	bool						mOpenTexPreview;
	bool						mBakeTextureEnabled;
    LLTextureCtrl::EPickInventoryType mInventoryPickType;
};

#endif  // LL_LLTEXTURECTRL_H
