/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

//==============================================================================
/**
    A file open/save dialog box.

    This is a Juce-based file dialog box; to use a native file chooser, see the
    FileChooser class.

    @code
    {
        wildcardFilter = std::make_unique<WildcardFileFilter> ("*.foo", String(), "Foo files");

        browser = std::make_unique<FileBrowserComponent> (FileBrowserComponent::canSelectFiles,
                                                          File(),
                                                          wildcardFilter.get(),
                                                          nullptr);

        dialogBox = std::make_unique<FileChooserDialogBox> ("Open some kind of file",
                                                            "Please choose some kind of file that you want to open...",
                                                            *browser,
                                                            false,
                                                            Colours::lightgrey);

        auto onFileSelected = [this] (int r)
        {
            modalStateFinished (r);

            auto selectedFile = browser->getSelectedFile (0);

            ...etc...
        };

        dialogBox->centreWithDefaultSize (nullptr);
        dialogBox->enterModalState (true,
                                    ModalCallbackFunction::create (onFileSelected),
                                    false);
    }
    @endcode

    @see FileChooser

    @tags{GUI}
*/
class JUCE_API  FileChooserDialogBox : public ResizableWindow,
                                       private FileBrowserListener
{
public:
    //==============================================================================
    /** Creates a file chooser box.

        @param title            the main title to show at the top of the box
        @param instructions     an optional longer piece of text to show below the title in
                                a smaller font, describing in more detail what's required.
        @param browserComponent a FileBrowserComponent that will be shown inside this dialog
                                box. Make sure you delete this after (but not before!) the
                                dialog box has been deleted.
        @param warnAboutOverwritingExistingFiles     if true, then the user will be asked to confirm
                                if they try to select a file that already exists. (This
                                flag is only used when saving files)
        @param backgroundColour the background colour for the top level window
        @param parentComponent  an optional component which should be the parent
                                for the file chooser. If this is a nullptr then the
                                dialog box will be a top-level window. AUv3s on iOS
                                must specify this parameter as opening a top-level window
                                in an AUv3 is forbidden due to sandbox restrictions.

        @see FileBrowserComponent, FilePreviewComponent
    */
    FileChooserDialogBox (const String& title,
                          const String& instructions,
                          FileBrowserComponent& browserComponent,
                          bool warnAboutOverwritingExistingFiles,
                          Colour backgroundColour,
                          Component* parentComponent = nullptr);

    /** Destructor. */
    ~FileChooserDialogBox() override;

    //==============================================================================
   #if JUCE_MODAL_LOOPS_PERMITTED
    /** Displays and runs the dialog box modally.

        This will show the box with the specified size, returning true if the user
        pressed 'ok', or false if they cancelled.

        Leave the width or height as 0 to use the default size
    */
    bool show (int width = 0, int height = 0);

    /** Displays and runs the dialog box modally.

        This will show the box with the specified size at the specified location,
        returning true if the user pressed 'ok', or false if they cancelled.

        Leave the width or height as 0 to use the default size.
    */
    bool showAt (int x, int y, int width, int height);
   #endif

    /** Sets the size of this dialog box to its default and positions it either in the
        centre of the screen, or centred around a component that is provided.
    */
    void centreWithDefaultSize (Component* componentToCentreAround = nullptr);

    //==============================================================================
    /** A set of colour IDs to use to change the colour of various aspects of the box.

        These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
        methods.

        @see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
    enum ColourIds
    {
        titleTextColourId      = 0x1000850, /**< The colour to use to draw the box's title. */
    };

private:
    class ContentComponent;
    ContentComponent* content;
    const bool warnAboutOverwritingExistingFiles;

    void closeButtonPressed();
    void selectionChanged() override;
    void fileClicked (const File&, const MouseEvent&) override;
    void fileDoubleClicked (const File&) override;
    void browserRootChanged (const File&) override;
    int getDefaultWidth() const;

    void okButtonPressed();
    void createNewFolder();
    void createNewFolderConfirmed (const String& name);

    static void createNewFolderCallback (int result, FileChooserDialogBox*, Component::SafePointer<AlertWindow>);

    ScopedMessageBox messageBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileChooserDialogBox)
};

} // namespace juce
