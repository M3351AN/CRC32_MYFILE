# CRC32_MYFILE
An interesting attempt to include the hash value of the file itself
<img width="1022" height="460" alt="fd93063e2a4bfe70bd083b9556b21ea6" src="https://github.com/user-attachments/assets/d1ade3ad-0a0b-458e-a379-d25faf427bc7" />

灰に潜り 秒針を噛み

白昼夢の中で ガンガン砕いた

## What this for?

As you can see, the file contains its own CRC32 hash.

It all started with a silly thought I had during a self-study class in middle school: How can a file contain its own hash value itself?

As we all know, digest algorithms (such as CRC32) are commonly used to verify file integrity and are widely used in communication protocols such as Ethernet frame checksum, PPP, and HDLC. In short, a modified file means a changed hash.

In middle school, I ran into this dilemma (even though it wasn't necessary): if I changed the file contents, the hash would change, so how could I include the hash in the file contents? We'd end up in a loop: file changed -> hash changed -> new hash written to the file -> file hash changed again...

But now, modern CPUs help me complete this loop which may be up to hundreds of millions of times.

## How this working?

As I said before, we try to replace the hash in a loop hundreds of millions of times, and then check the hash value of the file again, until we find a "fixed point": after we write the text with this hash value, the hash of the file is consistent with the text we wrote.

When I ran a test using the "crc32_myfile.cc" file included in the project, my i7-12700H burned through my adolescent unnecessary confusion at about 800,000 collisions per second, finally arriving at the result after 1467 seconds (roughly listening to ZTMY's "秒針を噛む" five times in total, followed by about 2 minutes and 57 seconds of the sixth listen).

## So what does this speed depend on?
- file size
- CPU performance
- luck

At least we've proven that CRC32 isn't always reliable. A fixed point was found within 1467 seconds. This means that, theoretically, a modified file can get the same CRC32 hash as the original file in about 1467 seconds.

1467 seconds, that's a long time, isn't it? But for someone with malicious intent, it's not a long time to complete in their lifetime, let alone just listening to six songs.

What about CRC64? I don't know. If the Paleolithic people could create 12700H, they would have found a fixed point in a file this morning. XD

Therefore, if you want to do file verification externally, please use a more complex, large, and collision-resistant digest algorithm such as CRC64\SHA256

If you want a file to verify itself, digital signing is obviously a wiser approach.

## Usage

- Have a CPU, preferably a "modern" one
- Place a "CRC32_HASH_MARK" in the file
- Then: crc32_myfile <thefile>
- Listen to "秒針を噛む" 6 times, 12 times, 24 times, 48 ​​times, 96 times... or take a nap
- Profit

## Credits
- [richgel999(Rich Geldreich)](https://github.com/richgel999/) for his [miniz](https://github.com/richgel999/miniz)

## License

This project is licensed under [**TOSSUCU 2025.9**](LICENSE).
```diff
+ You are free to:
    • Use: Utilize the software for any purpose not explicitly restricted
    • Copy: Reproduce the software without limitation
    • Modify: Create derivative works through remixing/transforming
    • Merge: Combine with other materials
    • Publish: Display/communicate the software publicly
    • Distribute: Share copies of the software
    • Sublicense: Grant sublicenses for original/modified material
    • Sell: Transfer or sell copies of the software

+ Under the following terms:
    • Attribution: Must include copyright notice and this license
      in all copies or substantial portions of the Software

+ Disclaimer:
    • The Software is provided "AS IS", without warranty of any kind
    • The right holder is not liable for any claim, damages, or other liability

- You are not allowed to:
    • Violate the license terms: If violated and causing damage to the right holder,
      legal liability may be pursued under applicable laws

```
**Note**: This section is a simplified summary. For the full legal terms, please refer to the [LICENSE](LICENSE) file.
