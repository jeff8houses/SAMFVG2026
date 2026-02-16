# SAMFVG2026
# Git & Wwise Collaboration Assignment

**Sound Museum Project**

---

## Assignment Overview

In this assignment, you'll collaborate on a shared Unity + Wwise project using Git and GitHub. You'll claim a sound sphere in the Sound Museum, add your own sound effect to the shared Wwise project, and submit your work via pull request.

### Learning Objectives

- Practice the fork → clone → branch → commit → push → pull request workflow
- Work collaboratively in a shared Wwise project
- Import audio and create Wwise events
- Implement interactive sound triggers in Unity
- Manage merge conflicts in a real-world team scenario

**Due Date:** [INSERT DATE]

---

## The Project: Sound Museum

The Unity scene is a gallery space with 12 numbered transparent spheres arranged around the room. Each sphere has a trigger zone - when the player walks into it, a sound plays. Your job is to claim one sphere and add your sound to the shared Wwise project.

Think of it like a museum exhibition where each sphere showcases a different sound design concept. Students can walk through the completed project and experience everyone's work.

### How Self-Assignment Works

**IMPORTANT:** To avoid merge conflicts, you must claim a sphere number that no one else has taken.

The claiming process:

1. Check the `CLAIMS.md` file in the repository to see which spheres are available
2. Choose an available sphere number (1-12)
3. Edit `CLAIMS.md` to add your name next to your chosen number
4. Commit and push this change FIRST, before adding your sound
5. Create a pull request for your claim
6. Wait for instructor approval (usually same-day)
7. Once approved, proceed with adding your sound

**Why this system?** By claiming your sphere in a separate pull request first, we ensure no two students accidentally work on the same sphere. It's like reserving a seat - first come, first served!

---

## What's Already in the Repository

The starter repository includes:

- Unity project with Sound Museum scene
- 12 numbered transparent sphere triggers
- Wwise integration package (already imported)
- **Shared Wwise project** with 12 pre-created Events (`Play_Sphere01` through `Play_Sphere12`)
- 12 Actor-Mixer folders in Wwise (one for each sphere)
- Generated soundbanks (`StudentSounds.bnk`)
- A C# script on each sphere for triggering sounds
- `CLAIMS.md` file for self-assignment
- Proper `.gitignore` for Unity and Wwise files

**KEY DIFFERENCE:** Unlike many Wwise tutorials, the Wwise project IS included in this repository. Everyone works in the same Wwise project - this is how real game studios collaborate!

---

## Assignment Requirements

You must complete the following tasks:

1. **Fork the repository** to your own GitHub account
2. **Clone your forked repository** to your local machine
3. **Create a branch** named `claim-sphere-[number]` (e.g., `claim-sphere-3`)
4. **Claim your sphere** by editing `CLAIMS.md` and creating a pull request
5. **Wait for approval,** then create a new branch: `add-[yourlastname]-sound`
6. **Open the shared Wwise project** (included in the repository)
7. **Import your sound file** into your assigned Actor-Mixer folder
8. **Link your sound** to your pre-assigned Event (e.g., `Play_Sphere03`)
9. **Generate soundbanks** in Wwise
10. **Test in Unity** to make sure the sound plays when you walk into your sphere
11. **Commit your changes** (Wwise project files + audio files + soundbanks)
12. **Push your branch** to your forked repository on GitHub
13. **Create a pull request** to the original repository with a description of your sound

---

## Understanding the Git Workflow

### CRITICAL CONCEPT

**You will work on feature branches and never touch your fork's main branch.**

### The Branch Workflow (Visual Guide)

```
[Instructor's Repository - main branch]
              ↑
              | Your pull request goes HERE
              |
[Your Fork - main branch] ← You NEVER work here!
              |
              └─ claim-sphere-3 ← You work here first
              └─ add-smith-sound ← Then you work here
```

**Key Points:**

- **Your fork's main branch stays untouched** - it's just a clean copy of the instructor's repo
- **All your work happens on feature branches** (`claim-sphere-X` and `add-yourname-sound`)
- **Pull requests go from your feature branch → instructor's main** (NOT to your own main)
- **After the instructor merges, your feature branch is done** - you can delete it

### Common Mistake to Avoid

❌ **WRONG:** Merging your feature branch into your own fork's main branch

✅ **CORRECT:** Pushing your feature branch and creating a pull request directly to the instructor's repo

**Why?** Keeping your fork's main branch clean makes it easy to sync with the instructor's updates. If everyone merges to their own main, chaos ensues when trying to get the latest changes.

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

---

### Part 2: Claim Your Sphere

#### Step 3: Create a Branch for Your Claim

1. In GitHub Desktop, click on **Current Branch** at the top
2. Click **New Branch**
3. Name your branch: `claim-sphere-[number]` (e.g., `claim-sphere-3`)
   - Choose any number 1-12 that looks available in `CLAIMS.md`
4. Click **Create Branch**

✓ **Checkpoint:** The top of GitHub Desktop should now show your new branch name (e.g., `claim-sphere-3`).

#### Step 4: Edit CLAIMS.md

1. Open the project folder in your file explorer
   - In GitHub Desktop, you can click **Repository → Show in Finder/Explorer**
2. Open `CLAIMS.md` in a text editor (Notepad, TextEdit, VS Code, etc.)
3. Find your chosen sphere number
4. Replace 'Available' with your full name

**Example - Change from:**
```
3. Available
```

**To:**
```
3. John Smith
```

5. Save the file and close the text editor

#### Step 5: Commit and Push Your Claim

1. Go back to GitHub Desktop
2. You should see `CLAIMS.md` in the changed files list
3. In the commit message box (bottom left), write:
   - **Summary:** `Claim sphere [number]`
4. Click **Commit to claim-sphere-[number]**
5. Click **Push origin** (top right)

✓ **Checkpoint:** Your claim branch is now on GitHub.

#### Step 6: Create Pull Request for Your Claim

1. In GitHub Desktop, click **Create Pull Request**
   - This opens your web browser
2. **Title:** `Claim sphere [number] - [Your Name]`
3. **Description:** Leave blank or write: "Claiming sphere [number] for my sound."
4. Click **Create Pull Request**
5. **Wait for instructor approval** (usually same day - check your GitHub notifications)

⏸️ **PAUSE HERE:** Do not proceed to Part 3 until your claim is approved. Once approved, the instructor will merge your claim and your sphere is officially yours!

---

### Part 3: Add Your Sound to the Wwise Project

**PREREQUISITE:** Your sphere claim has been approved and merged by the instructor.

#### Step 7: Update Your Local Repository

Before starting your sound work, sync your local repo with the latest changes:

1. In GitHub Desktop, switch to the **main** branch
   - Click **Current Branch → main**
2. Click **Fetch origin** (top right)
3. If updates are available, click **Pull origin**

✓ **Checkpoint:** Your local main branch now includes everyone's approved claims.

#### Step 8: Create Your Sound Branch

1. Make sure you're on the main branch (see step 7)
2. Click **Current Branch → New Branch**
3. Name it: `add-[yourlastname]-sound` (e.g., `add-smith-sound`)
4. Click **Create Branch**

✓ **Checkpoint:** You're now on your `add-[yourlastname]-sound` branch, ready to add your sound!

#### Step 9: Open the Shared Wwise Project

**IMPORTANT:** You will be opening the Wwise project that's already IN the repository. This is different from creating your own project!

1. Open **Wwise**
2. Go to **File → Open Project** (NOT New Project!)
3. Navigate to your cloned repository folder
4. Look for the **WwiseProject** folder
5. Open the `.wproj` file (should be named `SoundMuseum.wproj` or similar)

✓ **Checkpoint:** Wwise should open the project. You should see the Actor-Mixer Hierarchy with folders named `Sphere_01_Sounds` through `Sphere_12_Sounds`, and Events named `Play_Sphere01` through `Play_Sphere12`.

#### Step 10: Import Your Sound

1. In Wwise, go to the **Project Explorer** (left panel)
2. In the **Actor-Mixer Hierarchy** tab, find YOUR sphere's folder
   - If you claimed sphere 3, look for `Sphere_03_Sounds`
3. Right-click on your folder → **Import Audio Files** (or press Shift+I)
4. Browse to your audio file and select it
   - Use `.wav` or `.mp3` format
   - Can be from freesound.org, recorded yourself, or existing assets
   - Be creative! This is your sound exhibit
5. Click **Import**

✓ **Checkpoint:** You should now see a Sound SFX object in your folder with your audio file name.

#### Step 11: Link Your Sound to the Event

1. In the Project Explorer, switch to the **Events** tab (top of left panel)
2. Find YOUR event (e.g., `Play_Sphere03` if you're sphere 3)
3. Click on your event to select it
4. Look at the **Event Editor** panel (usually on the right)
5. You should see a **Target** section (might say 'Not connected' or be empty)
6. Go back to the **Actor-Mixer Hierarchy** tab
7. Find your Sound SFX object in your folder
8. Drag and drop your Sound SFX into the **Target** field of the Event Editor

✓ **Checkpoint:** The Target field should now show your sound's name. You can test it by clicking the Play button in the Transport Control!

#### Step 12: Save Your Wwise Project

1. Go to **File → Save Project** (or Ctrl+S / Cmd+S)

**IMPORTANT:** Always save before generating soundbanks!

#### Step 13: Generate Soundbanks

1. Go to the **SoundBanks** tab (bottom panel in Wwise)
2. In the left panel, you should see a soundbank called **StudentSounds**
3. Select **StudentSounds** (make sure it's checked)
4. Make sure **Windows** platform is selected (or your target platform)
5. Click **Generate Selected** (or press F7)

✓ **Checkpoint:** Wwise should say 'SoundBank generation succeeded' at the bottom. Your changes are now in the .bnk files!

**Note:** The soundbanks are already configured to generate to the correct Unity folder (`Assets/StreamingAssets/Audio/GeneratedSoundBanks/`).

---

### Part 4: Test in Unity

#### Step 14: Open the Unity Project

1. Open **Unity Hub**
2. If the project isn't already added, click **Add → Add project from disk**
3. Navigate to your cloned repository folder and select it
4. Open the project
   - First time opening may take a few minutes as Unity imports everything
5. Once loaded, open the **SoundMuseum** scene (should be in `Assets/Scenes/`)

#### Step 15: Find Your Sphere

1. In the **Hierarchy** panel (left side), expand the **Spheres** group
2. Find `Sphere_[your-number]` (e.g., `Sphere_03`)
3. Click on it to select it
4. In the **Inspector** panel (right side), you should see a **WwiseTrigger** script component

✓ **Checkpoint:** The Event Name field should already say `Play_Sphere03` (or your sphere number). You shouldn't need to change anything!

#### Step 16: Test Your Sound

1. Press the **Play** button at the top center of Unity
2. Use **WASD** keys to move the player character around
3. Walk into your transparent sphere
4. Your sound should play!

**⚠️ Troubleshooting:**

- If no sound plays, check the Unity Console for errors (**Window → General → Console**)
- Make sure you generated soundbanks in Wwise (Step 13)
- Verify you linked your sound to the correct Event in Wwise (Step 11)
- Check that you're triggering the correct sphere (the one you claimed)
- If still stuck, ask for help in class or on Discord!

5. Press the **Play** button again to exit play mode
6. If you made any changes in Unity, save your scene: **File → Save**

---

### Part 5: Commit and Push Your Changes

#### Step 17: Review Your Changes in GitHub Desktop

1. Open **GitHub Desktop**
2. You should see a list of changed files. Key files you'll see:
   - **Wwise project files** (`.wproj`, `.wsettings`, etc.)
   - **Your audio file** in `WwiseProject/Originals/SFX/Sphere_XX/`
   - **Generated soundbank** (`StudentSounds.bnk`)
   - **Possibly Unity scene file** (if you made any Unity changes)
3. Make sure ALL your changes are checked (should be by default)

⚠️ **If you see Library/ or Temp/ folders, STOP:** Something is wrong with the `.gitignore`. Ask for help before committing.

#### Step 18: Commit Your Changes

1. In the bottom left, write a commit message:
   - **Summary:** `Add [YourLastName] sound to sphere [number]`
   - **Description (optional):** Brief description of your sound (e.g., "Added door creak sound from freesound.org")
2. Click **Commit to add-[yourlastname]-sound**

✓ **Checkpoint:** The changes list should now be empty. Your changes are committed locally.

#### Step 19: Push to GitHub

1. At the top right, click **Push origin**
2. Wait for the upload to complete (may take a minute if your audio file is large)

✓ **Checkpoint:** Your `add-[yourlastname]-sound` branch is now on GitHub!

---

### Part 6: Create a Pull Request

#### Step 20: Open a Pull Request

1. In GitHub Desktop, click **Create Pull Request**
   - This will open GitHub in your web browser
2. On the pull request page, verify:
   - **Base repository:** `[instructor]/[repo-name]` ← base: main
   - **Head repository:** `[your-username]/[repo-name]` ← compare: add-yourlastname-sound

✓ **This is correct!** Your feature branch is going to the instructor's main, NOT your own main.

3. Write a descriptive title:
   - Example: "Add door creak sound to sphere 3 - Smith"
4. In the description box, include:
   - Which sphere number you worked on
   - A brief description of your sound
   - Where you got the sound (freesound.org, recorded it yourself, etc.)
   - Any issues you encountered or creative choices you made
5. Click **Create Pull Request**

✓ **Checkpoint:** Your pull request is now visible on the original repository! The instructor will review and merge it.

🎉 **YOU'RE DONE!** Great work! Once the instructor merges all pull requests, the entire class can walk through the Sound Museum and hear everyone's work.

---

## Submission

Your assignment is complete when:

- ✓ Your sphere claim pull request has been approved and merged
- ✓ Your sound pull request has been created on the original repository
- ✓ Your sound plays correctly in Unity (tested before submission)
- ✓ You have submitted the pull request URL on Canvas (if required by instructor)

**DO NOT:** Close or merge your own pull request. The instructor will review and merge all pull requests.

---

## Grading Criteria

Your assignment will be graded on:

- **Correct Git workflow (30 points):** Fork, branch, claim, branch again, commit, push, two pull requests completed correctly
- **Wwise implementation (30 points):** Audio imported correctly, event properly linked, soundbanks generated
- **Unity integration (30 points):** Sound plays when player enters correct sphere
- **Sound quality & appropriateness (10 points):** Sound is clear, audible, and demonstrates effort

**Total: 100 points**

---

## Common Issues & Solutions

### Issue: Someone already claimed my sphere

- Check `CLAIMS.md` - pick a different available sphere
- Delete your current claim branch and create a new one with the new number
- In GitHub Desktop: **Branch → Delete** → select your `claim-sphere-X` branch

### Issue: Sound doesn't play in Unity

- Check the Console for errors (**Window → General → Console**)
- Make sure you generated soundbanks in Wwise
- Verify you linked your sound to the correct Event (`Play_SphereXX`)
- Check that you're triggering the correct sphere
- Try re-generating soundbanks and restarting Unity

### Issue: I can't find the Wwise project

- Look for a folder called **WwiseProject** in your cloned repository
- The `.wproj` file should be inside that folder
- If missing, make sure you cloned the latest version of the repository

### Issue: Wwise says my audio file format is not supported

- Use `.wav` or `.mp3` format
- If you have a different format, convert it using Audacity (free)

### Issue: Merge conflict when creating pull request

- This can happen if multiple people edited the Wwise project at the same time
- Ask the instructor for help - they'll guide you through resolving it
- Usually it's safe to accept 'both changes' in the Wwise `.wproj` file

### Issue: GitHub Desktop shows hundreds of files changed

- If you see `Library/` or `Temp/` folders, DO NOT COMMIT
- The `.gitignore` may not be working - ask the instructor
- Some Wwise project files showing up is normal (they're small text files)

### Issue: I want to change my sound after submitting

- If your PR hasn't been merged yet, you can make changes on the same branch
- Import a new sound in Wwise, re-link the Event, generate soundbanks
- Commit and push again - the pull request will automatically update

---

## Additional Resources

- [GitHub Desktop Documentation](https://docs.github.com/en/desktop)
- [Wwise Documentation](https://www.audiokinetic.com/library/)
- [Git Visualizer](https://git-school.github.io/visualizing-git/)
- [Free Sound Effects - Freesound](https://freesound.org)
- [Unity Learn](https://learn.unity.com)
- [Audacity (free audio editor)](https://www.audacityteam.org)

**Questions?** Ask in class, on Discord/Slack, or email the instructor. Collaboration is part of learning - don't struggle alone!
