# Aseprite Sprite Creation Guide for Tamagotchi Gift

## Quick Start Setup

### Initial Aseprite Configuration
1. **Create New Sprite**
   - File → New (Ctrl/Cmd + N)
   - Width: 16 pixels
   - Height: 16 pixels
   - Color Mode: Indexed (best for embedded systems and small file sizes)
   - Background: Transparent

2. **Grid Settings** (Optional but helpful)
   - View → Grid → Grid Settings
   - Set to 16x16 to see your sprite boundaries clearly

## 16x16 Pixel Art Fundamentals

### Working at Small Scale
- **Every pixel counts** - At 16x16, you have only 256 pixels total
- **Use limited colors** - 4-8 colors per sprite keeps it readable
- **Focus on silhouette** - The outline shape should be recognizable
- **Avoid fine details** - They won't be visible at this size

### Color Palette Tips
1. Create a unified palette for your entire project (8-16 colors)
2. Include:
   - 2-3 shades for your character
   - 2 shades for outlines/shadows
   - 2-3 background colors
   - Accent colors for emotions/effects

## Animation Workflow (2-4 Frames)

### Creating Frame Animations

1. **Timeline Setup**
   - The timeline is at the bottom of Aseprite
   - Each frame shows as a numbered cell
   - Default frame duration: 100ms (adjust as needed)

2. **Adding Frames**
   - Click "New Frame" button (or Alt + N)
   - Create 2-4 frames based on animation complexity

3. **Animation Types for Your Game**

   **Idle Animation (2 frames)**
   - Frame 1: Base pose
   - Frame 2: Slight variation (e.g., blink, small movement)
   - Duration: 500-800ms per frame (slow, subtle)

   **Active Animation (3-4 frames)**
   - Frame 1: Start position
   - Frame 2-3: Middle motion
   - Frame 4: End position (can loop to frame 1)
   - Duration: 100-200ms per frame (faster)

4. **Onion Skin** (Highly Recommended)
   - View → Onion Skin (or F3)
   - Shows previous/next frames as ghost images
   - Helps maintain consistency between frames

## Sprite Priority List (2 Week Timeline)

### Week 1: Core Assets

**Day 1-2: Character Sprites**
- [ ] Idle state (2 frames)
- [ ] Happy expression (2 frames)
- [ ] Sad expression (2 frames)
- [ ] Eating (3 frames)
- [ ] Sleeping (2 frames)

**Day 3-4: UI Elements**
- [ ] Hearts/love meter icons (16x16)
- [ ] Food items (16x16 each, static or 2 frames)
- [ ] Status icons (hungry, sleepy, etc.)
- [ ] Menu cursor/selector

**Day 5-7: Backgrounds**
- [ ] Home screen background (can be larger, like 128x64 for full screen)
- [ ] Minigame backgrounds (2-3 simple ones)
- [ ] Night/day variants if needed

### Week 2: Minigames & Polish

**Day 8-10: Minigame Sprites**
(Based on your minigames folder)
- [ ] Jump game: platforms, character jumping (3 frames)
- [ ] Catch game: falling objects (2 frames), basket/character
- [ ] Dance/music: character dancing (4 frames), music notes
- [ ] Candle: candle sprite, fire animation (3 frames)
- [ ] Fortune cookie: cookie (2 frames - closed/open)

**Day 11-12: Effects & Particles**
- [ ] Hearts (2 frames)
- [ ] Stars (2 frames)
- [ ] Sparkles (3 frames)
- [ ] Zzz sleep icons (2 frames)

**Day 13-14: Buffer & Polish**
- [ ] Fix any issues
- [ ] Test all sprites in-game
- [ ] Optimize file sizes if needed

## Step-by-Step: Creating Your First Sprite

### Example: Idle Character Animation

1. **Frame 1 - Base Pose**
   ```
   - Use Pencil tool (B key)
   - Draw basic character outline in dark color
   - Fill with main color (Paint Bucket: G key)
   - Add eyes, simple features
   - Keep it centered in the 16x16 canvas
   ```

2. **Frame 2 - Variation**
   ```
   - Click "New Frame" (or Alt + N)
   - Enable Onion Skin (F3)
   - Make small changes:
     * Close eyes for blink, OR
     * Move 1 pixel up/down for bounce
   - Keep 90% of pixels the same
   ```

3. **Preview**
   - Press Enter to play animation
   - Adjust frame timing if needed (right-click frame → Frame Properties)

4. **Save**
   - File → Save As (Shift + Ctrl/Cmd + S)
   - Name: `character_idle.aseprite` (keep .aseprite for editing later)

## Exporting Sprites for Your Game

### Export Individual Sprites

**For Static Sprites:**
1. File → Export → Export Sprite Sheet
2. Settings:
   - Layout: Single frame or Horizontal/Vertical strip
   - Output File: PNG or BMP
   - Include: "Merge duplicates" (reduces file size)

**For Animated Sprites:**
1. File → Export → Export Sprite Sheet
2. Settings:
   - Layout: Horizontal Strip (all frames in one row)
   - Output: `sprite_name.png`
   - Optional: Export JSON/Array for frame data

### For Embedded Systems (Arduino/PlatformIO)
You'll likely need to convert sprites to byte arrays:

1. Export as PNG (1-bit or indexed color)
2. Use online converters or tools like:
   - [image2cpp](http://javl.github.io/image2cpp/)
   - LCD Image Converter
   
3. Copy byte array into your `sprites.h` file

## Pro Tips for Speed & Efficiency

### Keyboard Shortcuts (Learn These!)
- **B** - Pencil/Brush
- **G** - Paint Bucket
- **I** - Eyedropper (pick colors quickly)
- **M** - Rectangular Marquee (select/move areas)
- **Alt + N** - New Frame
- **Enter** - Play/Stop animation
- **Z** - Zoom In, **Alt + Z** - Zoom Out
- **Alt + Mouse wheel** - Quick zoom
- **Tab** - Show/hide panels for more space

### Time-Saving Techniques
1. **Reuse and modify** - Copy idle animation as base for other animations
2. **Symmetry** - Edit → Flip Horizontal for symmetrical sprites
3. **Layers** - Use layers for complex sprites (background, character, effects)
4. **Tags** - Use animation Tags to organize different animations in one file
5. **Reference** - Keep window of similar pixel art open for inspiration

### Common Mistakes to Avoid
- ❌ Too many colors (stick to 4-8 per sprite)
- ❌ Too much detail (you only have 16x16!)
- ❌ Unreadable silhouette (test sprite at actual size)
- ❌ Inconsistent style between sprites
- ❌ Frame changes too drastic (smooth transitions)

## Testing Your Sprites

1. **Zoom Out** - View → Zoom → 100% to see actual size
2. **Test on target device** - If possible, export and test early
3. **Check contrast** - Works on both light and dark backgrounds?
4. **Animation speed** - 100-200ms for action, 500-800ms for idle

## Resources for Learning & Inspiration

### Tutorials
- Aseprite official tutorials (Help → Tutorials)
- MortMort on YouTube - pixel art tutorials
- Brandon James Greer - animation tutorials

### Inspiration (16x16 sprites)
- Look up "16x16 pixel art" on Pinterest
- Pokémon Gen 1-2 sprites (similar scale)
- Tamagotchi original sprites (your reference!)

### Color Palettes
- [Lospec Palettes](https://lospec.com/palette-list) - Filter by color count
- Recommend: AAP-64, Sweetie 16, or PICO-8 palette

## Final Checklist Before Starting

- [ ] Aseprite installed and opened
- [ ] Decided on main color palette (8-16 colors)
- [ ] Have reference images of tamagotchis/pixel art
- [ ] Created project folder structure for sprites
- [ ] Understand which sprites are highest priority
- [ ] Set daily goals from timeline above

## Remember

You have 2 weeks - that's plenty of time for 16x16 sprites! Don't aim for perfection on first try. Create rough versions of all essential sprites first, then polish. Simple, charming sprites are better than half-finished detailed ones.

**Start with your character's idle animation today. The rest will follow!**

Good luck! 🎨✨
