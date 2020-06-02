download a pre-compiled graph here (place in same folder as script): https://drive.google.com/file/d/1vno0I6F3jE5Tpn2JYFljKEE0he6uw7lV/view?usp=sharing (max 28 ess turns in a single run)

modify `starting_angles` to have accessible angles (usually cardinals and nearby walls)

modify `destination_angles` with any angle you'd like to find (will change this later to find all instead of stop of first find)

modify `types` with the allowed movement options

modify `max_ess` to what you'd like to do max per section of a setup


There is an html version available in [js/oot-angle-finder.html]().

To build it:
* Make sure you have typescript installed: `npm i -g typescript`
* `tsc`
* `cd js && python3 build_all_in_one.py`

To develop, you can use [js/use_ts.html]() and `tsc --watch`