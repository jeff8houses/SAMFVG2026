# SAMFVG2026
# Git & Wwise Collaboration Assignment

---

## Assignment Overview

In this assignment, you'll collaborate on a shared Unity + Wwise project using Git and GitHub. You'll create your own sound sphere in the Sound Museum, add your sound effect to your designated folder in the shared Wwise project, and submit your work via pull request.

### Learning Objectives

- Practice the fork → clone → branch → commit → push → pull request workflow
- Work collaboratively in a shared Wwise project
- Import audio and create Wwise events
- Implement interactive sound triggers in Unity
- Manage merge conflicts in a real-world team scenario

**Due Date:** [INSERT DATE]

---

## The Project: Sound Museum

The Unity scene is a gallery space where students create transparent spheres with sound triggers. Each sphere has a trigger zone - when the player walks into it, a sound plays. You'll create your own sphere and add your sound to the shared Wwise project.

Think of it like a museum exhibition where each sphere showcases a different sound design concept. Students can walk through the completed project and experience everyone's work.

**An example sphere is already in the scene** to show you the setup!

---

## What's Already in the Repository

The starter repository includes:

- Unity project with Sound Museum scene
- **Example sphere** showing the complete setup
- Wwise integration package (already imported)
- **Shared Wwise project** with organized folder structure
- **Student folders** in Wwise:
  - Audio folders (one per student under Actor-Mixer Hierarchy)
  - Event folders (one per student under Events)
- Generated soundbanks (`StudentSounds.bnk`)
- Proper `.gitignore` for Unity and Wwise files

**KEY DIFFERENCE:** Unlike many Wwise tutorials, the Wwise project IS included in this repository. Everyone works in the same Wwise project - this is how real game studios collaborate!

---

## Your Designated Wwise Folders

**IMPORTANT:** You will ONLY work in folders with your last name. This prevents conflicts!

In the Wwise project, you'll find:

### Under Actor-Mixer Hierarchy → StudentSounds:
- `Smith/` ← Your audio goes here
- `Johnson/` 
- `Garcia/`
- etc.

### Under Events → StudentEvents:
- `Smith/` ← Your events go here
- `Johnson/`
- `Garcia/`
- etc.

**Stay in your folder!** Don't create anything outside your designated folders or modify other students' work.

---

## Assignment Requirements

You must complete the following tasks:

1. **Fork the repository** to your own GitHub account
2. **Clone your forked repository** to your local machine
3. **Create a branch** named `add-[yourlastname]-sound` (e.g., `add-smith-sound`)
4. **Open the shared Wwise project** (included in the repository)
5. **Import your sound file** into YOUR designated Audio folder
6. **Create a Play Event** in YOUR designated Event folder
7. **Link your sound** to your Event
8. **Generate soundbanks** in Wwise
9. **Create a sphere in Unity** (duplicate the example sphere)
10. **Configure the trigger** to play your event
11. **Test in Unity** to make sure the sound plays when you walk into your sphere
12. **Commit your changes** (Wwise project files + audio files + soundbanks + Unity scene)
13. **Push your branch** to your forked repository on GitHub
14. **Create a pull request** to the original repository with a description of your sound

---

## Understanding the Git Workflow

### CRITICAL CONCEPT

**You will work on a feature branch and never touch your fork's main branch.**

### The Branch Workflow (Visual Guide)

```
[Instructor's Repository - main branch]
              ↑
              | Your pull request goes HERE
              |
[Your Fork - main branch] ← You NEVER work here!
              |
              └─ add-smith-sound ← You work here
```

**Key Points:**

- **Your fork's main branch stays untouched** - it's just a clean copy of the instructor's repo
- **All your work happens on your feature branch** (`add-yourname-sound`)
- **Pull requests go from your feature branch → instructor's main** (NOT to your own main)
- **After the instructor merges, your feature branch is done** - you can delete it

### Common Mistake to Avoid

❌ **WRONG:** Merging your feature branch into your own fork's main branch

✅ **CORRECT:** Pushing your feature branch and creating a pull request directly to the instructor's repo

**Why?** Keeping your fork's main branch clean makes it easy to sync with the instructor's updates.

---

## Detailed Step-by-Step Instructions

### Part 1: Fork and Clone the Repository

#### Step 1: Fork the Repository

1. Open your web browser and navigate to the original repository: [INSERT REPO URL]
2. Click the **Fork** button in the upper right corner of the page
3. GitHub will create a copy of the repository under your account
4. You should now be viewing YOUR fork at: `github.com/[your-username]/[repo-name]`

#### Step 2: Clone Your Fork to Your Computer

1. Open **GitHub Desktop**
2. Go to **File → Clone Repository**
3. Select your forked repository from the list (it should appear under 'Your Repositories')
4. Choose where to save it on your computer (remember this location!)
5. Click **Clone**

✓ **Checkpoint:** You should now have a local copy of the project on your computer, including the Unity project AND the Wwise project.

#### Step 3: Create Your Feature Branch

1. In GitHub Desktop, click on **Current Branch** at the top
2. Click **New Branch**
3. Name it: `add-[yourlastname]-sound` (e.g., `add-smith-sound`)
4. Click **Create Branch**

✓ **Checkpoint:** You're now on your `add-[yourlastname]-sound` branch, ready to add your sound!

---

### Part 2: Add Your Sound to the Wwise Project

#### Step 4: Open the Shared Wwise Project

**IMPORTANT:** You will be opening the Wwise project that's already IN the repository. This is different from creating your own project!

1. Open **Wwise**
2. Go to **File → Open Project** (NOT New Project!)
3. Navigate to your cloned repository folder
4. Look for the **WwiseProject** folder
5. Open the `.wproj` file (should be named `SoundMuseum.wproj` or similar)

✓ **Checkpoint:** Wwise should open the project. You should see folder structure under Actor-Mixer Hierarchy → StudentSounds and Events → StudentEvents with student last names.

#### Step 5: Import Your Sound

1. In Wwise, go to the **Project Explorer** (left panel)
2. In the **Actor-Mixer Hierarchy** tab, expand **StudentSounds**
3. Find YOUR folder (the one with your last name)
4. Right-click on your folder → **Import Audio Files** (or press Shift+I)
5. Browse to your audio file and select it
   - Use `.wav` or `.mp3` format
   - Can be from freesound.org, recorded yourself, or existing assets
   - Be creative! This is your sound exhibit
6. Click **Import**

✓ **Checkpoint:** You should now see a Sound SFX object in your folder with your audio file name.

#### Step 6: Create Your Play Event

1. In the Project Explorer, switch to the **Events** tab (top of left panel)
2. Expand **StudentEvents**
3. Find YOUR folder (the one with your last name)
4. Right-click on your folder → **New Child → Play**
5. Name your event: `Play_[YourLastName]` (e.g., `Play_Smith`)

✓ **Checkpoint:** You should now have an event in your Events folder.

#### Step 7: Link Your Sound to the Event

1. With your new event selected, look at the **Event Editor** panel (usually on the right)
2. You should see a **Target** section (might say 'Not connected' or be empty)
3. Go back to the **Actor-Mixer Hierarchy** tab
4. Find your Sound SFX object in YOUR folder under StudentSounds
5. Drag and drop your Sound SFX into the **Target** field of the Event Editor

✓ **Checkpoint:** The Target field should now show your sound's name. You can test it by clicking the Play button in the Transport Control!

#### Step 8: Save Your Wwise Project

1. Go to **File → Save Project** (or Ctrl+S / Cmd+S)

**IMPORTANT:** Always save before generating soundbanks!

#### Step 9: Generate Soundbanks

1. Go to the **SoundBanks** tab (bottom panel in Wwise)
2. In the left panel, you should see a soundbank called **StudentSounds**
3. Select **StudentSounds** (make sure it's checked)
4. Make sure **Windows** platform is selected (or your target platform)
5. Click **Generate Selected** (or press F7)

✓ **Checkpoint:** Wwise should say 'SoundBank generation succeeded' at the bottom. Your changes are now in the .bnk files!

**Note:** The soundbanks are already configured to generate to the correct Unity folder (`Assets/StreamingAssets/Audio/GeneratedSoundBanks/`).

---

### Part 3: Create Your Sphere in Unity

#### Step 10: Open the Unity Project

1. Open **Unity Hub**
2. If the project isn't already added, click **Add → Add project from disk**
3. Navigate to your cloned repository folder and select it
4. Open the project
   - First time opening may take a few minutes as Unity imports everything
5. Once loaded, open the **SoundMuseum** scene (should be in `Assets/Scenes/`)

#### Step 11: Study the Example Sphere

1. In the **Hierarchy** panel (left side), find **ExampleSphere**
2. Click on it to select it
3. In the **Inspector** panel (right side), note the components:
   - **Sphere Collider** (with "Is Trigger" checked)
   - **WwiseTrigger** script (with an event name filled in)
4. This is the setup you'll replicate!

#### Step 12: Create Your Sphere

1. Right-click on **ExampleSphere** in the Hierarchy
2. Select **Duplicate** (or press Ctrl+D / Cmd+D)
3. Rename the duplicate to `[YourLastName]Sphere` (e.g., `SmithSphere`)
4. With your sphere selected, in the **Inspector**, find the **Transform** component
5. Change the **Position** to place your sphere somewhere else in the scene
   - Just change the X and Z values to move it around (e.g., X: 5, Z: 3)
   - Keep Y at around 1 so it's at ground level

#### Step 13: Configure Your Sphere's Trigger

1. With your sphere still selected, scroll down in the Inspector to the **WwiseTrigger** component
2. In the **Event Name** field, enter your event name: `Play_[YourLastName]`
   - Example: `Play_Smith`
   - This must match EXACTLY what you named your event in Wwise (case-sensitive!)

✓ **Checkpoint:** Your sphere is now configured with your event!

#### Step 14: Make Your Sphere Visible (Optional)

The example sphere is transparent, but you can customize yours:

1. With your sphere selected, look at the **Mesh Renderer** component in the Inspector
2. You can change the material or add a different color if you want
3. Or keep it transparent like the example!

#### Step 15: Test Your Sound

1. Press the **Play** button at the top center of Unity
2. Use **WASD** keys to move the player character around
3. Walk into your sphere
4. Your sound should play!

**⚠️ Troubleshooting:**

- If no sound plays, check the Unity Console for errors (**Window → General → Console**)
- Make sure you generated soundbanks in Wwise (Step 9)
- Verify you linked your sound to your Event in Wwise (Step 7)
- Check that the event name in Unity matches EXACTLY (case-sensitive!)
- Make sure you saved your Wwise project before generating soundbanks
- If still stuck, ask for help in class or on Discord!

5. Press the **Play** button again to exit play mode
6. Save your scene: **File → Save**

---

### Part 4: Commit and Push Your Changes

#### Step 16: Review Your Changes in GitHub Desktop

1. Open **GitHub Desktop**
2. You should see a list of changed files. Key files you'll see:
   - **Wwise project files** (`.wproj`, `.wsettings`, etc.)
   - **Your audio file** in `WwiseProject/Originals/SFX/[YourLastName]/`
   - **Generated soundbank** (`StudentSounds.bnk`)
   - **Unity scene file** (`SoundMuseum.unity`)
3. Make sure ALL your changes are checked (should be by default)

⚠️ **If you see Library/ or Temp/ folders, STOP:** Something is wrong with the `.gitignore`. Ask for help before committing.

#### Step 17: Commit Your Changes

1. In the bottom left, write a commit message:
   - **Summary:** `Add [YourLastName] sound sphere`
   - **Description (optional):** Brief description of your sound (e.g., "Added door creak sound from freesound.org")
2. Click **Commit to add-[yourlastname]-sound**

✓ **Checkpoint:** The changes list should now be empty. Your changes are committed locally.

#### Step 18: Push to GitHub

1. At the top right, click **Push origin**
2. Wait for the upload to complete (may take a minute if your audio file is large)

✓ **Checkpoint:** Your `add-[yourlastname]-sound` branch is now on GitHub!

---

### Part 5: Create a Pull Request

#### Step 19: Open a Pull Request

1. In GitHub Desktop, click **Create Pull Request**
   - This will open GitHub in your web browser
2. On the pull request page, verify:
   - **Base repository:** `[instructor]/[repo-name]` ← base: main
   - **Head repository:** `[your-username]/[repo-name]` ← compare: add-yourlastname-sound

✓ **This is correct!** Your feature branch is going to the instructor's main, NOT your own main.

3. Write a descriptive title:
   - Example: "Add door creak sound sphere - Smith"
4. In the description box, include:
   - A brief description of your sound
   - Where you got the sound (freesound.org, recorded it yourself, etc.)
   - The approximate location of your sphere in the scene (so the instructor can find it easily!)
   - Any issues you encountered or creative choices you made
5. Click **Create Pull Request**

✓ **Checkpoint:** Your pull request is now visible on the original repository! The instructor will review and merge it.

🎉 **YOU'RE DONE!** Great work! Once the instructor merges all pull requests, the entire class can walk through the Sound Museum and hear everyone's work.

---

## Submission

Your assignment is complete when:

- ✓ Your sound pull request has been created on the original repository
- ✓ Your sound plays correctly in Unity (tested before submission)
- ✓ Your sphere is positioned somewhere in the scene (not overlapping the example)
- ✓ You have submitted the pull request URL on Canvas (if required by instructor)

**DO NOT:** Close or merge your own pull request. The instructor will review and merge all pull requests.

---

## Working in Your Designated Folders

**This is crucial to avoid conflicts!**

### ✅ DO:
- Work ONLY in your designated Wwise folders (both Audio and Events)
- Create your sphere as a duplicate of the example
- Position your sphere away from others
- Use your last name for naming (sphere, event, etc.)

### ❌ DON'T:
- Create folders outside your designated area in Wwise
- Modify other students' audio or events
- Delete or move the example sphere
- Work directly in the root of StudentSounds or StudentEvents

### About Merge Conflicts

**You will likely encounter some merge conflicts** when multiple students work on the Unity scene simultaneously. This is normal and part of learning Git!

**Common conflicts:**
- Unity scene file (when multiple people add spheres)
- Wwise project file (when multiple people work at once)

**What to do:**
- Ask the instructor for help - we'll resolve them together
- This is a valuable learning experience!
- Most conflicts in the Wwise `.wproj` file can be resolved by accepting "both changes"

**The folder structure minimizes conflicts in Wwise** - since everyone works in their own folder, Wwise conflicts are usually easy to resolve.

---

## Common Issues & Solutions

### Issue: Sound doesn't play in Unity

- Check the Console for errors (**Window → General → Console**)
- Make sure you generated soundbanks in Wwise
- Verify you linked your sound to your Event in Wwise
- Check that the event name in Unity matches EXACTLY (case-sensitive!)
- Try re-generating soundbanks and restarting Unity

### Issue: I can't find the Wwise project

- Look for a folder called **WwiseProject** in your cloned repository
- The `.wproj` file should be inside that folder
- If missing, make sure you cloned the latest version of the repository

### Issue: I can't find my designated folders in Wwise

- Under Actor-Mixer Hierarchy, expand **StudentSounds**
- Under Events, expand **StudentEvents**
- Your folder should be named with your last name
- If it's not there, ask the instructor to add it

### Issue: Wwise says my audio file format is not supported

- Use `.wav` or `.mp3` format
- If you have a different format, convert it using Audacity (free)

### Issue: Merge conflict when creating pull request

- This is normal when multiple people edit the Unity scene!
- Ask the instructor for help - we'll resolve it together
- Don't try to resolve it yourself if you're not comfortable with merge conflicts

### Issue: GitHub Desktop shows hundreds of files changed

- If you see `Library/` or `Temp/` folders, DO NOT COMMIT
- The `.gitignore` may not be working - ask the instructor
- Some Wwise project files showing up is normal (they're small text files)

### Issue: I can't see my sphere in Unity

- Check the Hierarchy - make sure you created it
- Check the Transform position - it might be far away from the camera
- Click on your sphere in the Hierarchy and press **F** to focus the camera on it

### Issue: I want to change my sound after submitting

- If your PR hasn't been merged yet, you can make changes on the same branch
- Import a new sound in Wwise, re-link the Event, generate soundbanks
- Save the Unity scene if you moved your sphere
- Commit and push again - the pull request will automatically update

---

## Tips for Success

- **Start early** - Don't wait until the last minute in case you encounter issues
- **Test thoroughly** - Make sure your sound plays before submitting
- **Stay in your folders** - This is the key to avoiding conflicts
- **Communicate** - If you're stuck, ask for help!
- **Be creative** - This is your chance to showcase your sound design skills
- **Have fun** - You're building a collaborative sound art installation!
