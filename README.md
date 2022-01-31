# TrickyAutomation

A tiny plugin for automation of some routine tasks in UE4.

## Installation

1. Create Plugin folder in your project directory;

2. Download and unzip archive with the plugin;

3. Alternatively you can clone this repository into the pluging directory;

## How to open editor widgets

1. Find `EUW_AssetOrganizer` and `EUW_WorldOrganizer` in the plugin directory in the content browser;

2. Press Right Mouse Button on them;

3. Choose Run Utility Widget option;

## How to use

`EUW_AssetOrganizer` is for working with assets in content browser.

`EUW_WorldOrganizer` is for working with actors placed in the opened level.

### Asset Organizer

![Assets organizer](Images/img_assetorganizer.png)

With this widget you can:

1. Rename chosen assets. This action also adds Prefixes to the name automatically;

2. Add prefix to chosen assets;

3. Add suffix to chosen assets;

4. Find and replace given string in the names of chosen assets;

5. Delete or move to a bin folder unused assets;

You can change prefixes and suffixes inside the widget `EUW_AO_Rename` which you can find in the folder `TrickyAutomation/BaseWidgets/AssetOrginizer`.

### World Organizer

![World organizer](Images/img_worldorganizer.png)

With this widget you can:

1. Rename chose actors;

2. Find and replace given string in the names of chosen or all actors;

3. Move selected assets to a specific folder;

4. Select actors by given string;

5. Select actors by class;
