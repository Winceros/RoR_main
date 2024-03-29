/*
This source file is part of Rigs of Rods
Copyright 2005-2012 Pierre-Michel Ricordel
Copyright 2007-2012 Thomas Fischer

For more information, see http://www.rigsofrods.com/

Rigs of Rods is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 3, as
published by the Free Software Foundation.

Rigs of Rods is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Rigs of Rods.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifdef USE_MYGUI

#ifndef __SELECTOR_WINDOW_H__
#define __SELECTOR_WINDOW_H__

#include "RoRPrerequisites.h"
#include "Singleton.h"
#include "mygui/BaseLayout.h"
#include <Ogre.h>
#include "skin.h"


ATTRIBUTE_CLASS_LAYOUT(SelectorWindow, "SelectorWindow.layout");

class SelectorWindow :
	public wraps::BaseLayout,
	public Singleton2<SelectorWindow>
{
	friend class Singleton2<SelectorWindow>;
	SelectorWindow();
	~SelectorWindow();
public:

	void setupCamera(Ogre::Camera* _camera) { mCamera = _camera; }

	bool isFinishedSelecting();
	enum LoaderType {LT_None, LT_Terrain, LT_Vehicle, LT_Truck, LT_Car, LT_Boat, LT_Airplane, LT_Trailer, LT_Load, LT_Extension, LT_Network, LT_NetworkWithBoat, LT_Heli, LT_SKIN, LT_AllBeam};
	void show(LoaderType type);
	void hide();

	Cache_Entry *getSelection() { return mSelectedTruck; }
	Skin *getSelectedSkin() { return mSelectedSkin; }
	std::vector<Ogre::String> getTruckConfig() { return mTruckConfigs; }
	void setEnableCancel(bool enabled);
private:
	// gui events
	void eventKeyButtonPressed_Main(MyGUI::WidgetPtr _sender, MyGUI::KeyCode _key, MyGUI::Char _char);
	void eventComboChangePositionTypeComboBox(MyGUI::ComboBoxPtr _sender, size_t _index);
	void eventListChangePositionModelList(MyGUI::ListPtr _sender, size_t _index);
	void eventListChangePositionModelListAccept(MyGUI::ListPtr _sender, size_t _index);
	void eventComboAcceptConfigComboBox(MyGUI::ComboBoxPtr _sender, size_t _index);
	void eventMouseButtonClickOkButton(MyGUI::WidgetPtr _sender);
	void eventMouseButtonClickCancelButton(MyGUI::WidgetPtr _sender);
	void eventSearchTextChange(MyGUI::EditBox *_sender);
	void eventSearchTextGotFocus(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr oldWidget);
	void notifyWindowChangeCoord(MyGUI::Window* _sender);
	void resizePreviewImage();

	// other functions
	void getData();
	void onCategorySelected(int categoryID);
	void onEntrySelected(int entryID);
	void selectionDone();
	bool searchCompare(Ogre::String searchString, Cache_Entry *ce);

	void updateControls(Cache_Entry *entry);
	void setPreviewImage(Ogre::String texture);

	std::vector<Cache_Entry> mEntries;
	std::map<int, int> mCategoryUsage;
	std::vector<Skin *> mCurrentSkins;
	LoaderType mLoaderType;
	bool mSelectionDone;
	std::vector<Ogre::String> mTruckConfigs;
	Ogre::Camera *mCamera;
	Cache_Entry *mSelectedTruck;
	int visibleCounter;
	Skin *mSelectedSkin;
	Ogre::String lastImageTextureName;
private:
	ATTRIBUTE_FIELD_WIDGET_NAME(SelectorWindow, mTypeComboBox, "Type");
	MyGUI::ComboBox* mTypeComboBox;
	ATTRIBUTE_FIELD_WIDGET_NAME(SelectorWindow, mModelList, "Model");
	MyGUI::ListBox* mModelList;
	ATTRIBUTE_FIELD_WIDGET_NAME(SelectorWindow, mEntryNameStaticText, "EntryName");
	MyGUI::TextBox* mEntryNameStaticText;
	ATTRIBUTE_FIELD_WIDGET_NAME(SelectorWindow, mEntryDescriptionStaticText, "EntryDescription");
	MyGUI::TextBox* mEntryDescriptionStaticText;
	ATTRIBUTE_FIELD_WIDGET_NAME(SelectorWindow, mConfigComboBox, "Config");
	MyGUI::ComboBox* mConfigComboBox;
	ATTRIBUTE_FIELD_WIDGET_NAME(SelectorWindow, mPreviewStaticImage, "Preview");
	MyGUI::ImageBox* mPreviewStaticImage;
	ATTRIBUTE_FIELD_WIDGET_NAME(SelectorWindow, mOkButton, "Ok");
	MyGUI::Button* mOkButton;
	ATTRIBUTE_FIELD_WIDGET_NAME(SelectorWindow, mCancelButton, "Cancel");
	MyGUI::Button* mCancelButton;
	ATTRIBUTE_FIELD_WIDGET_NAME(SelectorWindow, mSearchLineEdit, "SearchLine");
	MyGUI::EditBox* mSearchLineEdit;
	ATTRIBUTE_FIELD_WIDGET_NAME(SelectorWindow, mPreviewStaticImagePanel, "PreviewBox");
	MyGUI::Widget* mPreviewStaticImagePanel;
};

#endif // __SELECTOR_WINDOW_H__

#endif //MYGUI
