# CanStack

A simple plugin that generates can stacks from an input object.

Clones of an input object are stacked pyramid-syle, like food cans.

This plugin demonstrates the following C4D API aspects:
* Generator object plugins with input, derived from `class ObjectData`
* Cloning objects as 'real' clones or render instances
* Generator caching with `CompareDependenceList()`, `DIRTYFLAGS` and recursive dirty checks
* Adjusting container values with `MSG_DESCRIPTION_VALIDATE`
* Using command buttons in the attribute manager with `MSG_DESCRIPTION_COMMAND`
* Using BaseArrays and Iterators
* Vector and matrix math in general
